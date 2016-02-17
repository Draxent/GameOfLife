/**
 *	@file functions.cpp
 *  @brief Implementation of the functions used in the main.cpp file.
 *  @author Federico Conte (draxent)
 *
 *	Copyright 2015 Federico Conte
 *	https://github.com/Draxent/GameOfLife
 *
 *	Licensed under the Apache License, Version 2.0 (the "License");
 *	you may not use this file except in compliance with the License.
 *	You may obtain a copy of the License at
 *
 *	http://www.apache.org/licenses/LICENSE-2.0
 *
 *	Unless required by applicable law or agreed to in writing, software
 *	distributed under the License is distributed on an "AS IS" BASIS,
 *	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *	See the License for the specific language governing permissions and
 *	limitations under the License.
 */

#include "../include/functions.h"

void compute_generation( Grid* g, int* numsNeighbours, size_t start, size_t end, bool vectorization )
{
	if ( vectorization )
	{
		__assume_aligned( g->Read, 64 );
		__assume_aligned( g->Write, 64 );
		__assume_aligned( numsNeighbours, 64 );
		for ( size_t pos = start; pos < end; pos += VLEN )
		{
			// Save the computation of the neighbours counting into the numsNeighbours array.
			numsNeighbours[0:VLEN] = g->countNeighbours( pos, __sec_implicit_index(0) );
			// Box ← (( #Neighbours == 3 ) OR ( Cell is alive AND #Neighbours == 2 )) in vector notations.
			g->Write[pos:VLEN] = ( numsNeighbours[0:VLEN] == 3 || ( g->Read[pos:VLEN] && numsNeighbours[0:VLEN] == 2 ) );
		}
	}
	else
	{
		for ( size_t pos = start; pos < end; pos++ )
		{
			// Calculate #Neighbours.
			int numsNeighbor = g->countNeighbours( pos, 0 );
			// Box ← (( #Neighbours == 3 ) OR ( Cell is alive AND #Neighbours == 2 )).
			g->Write[pos] = ( numsNeighbor == 3 || ( g->Read[pos] && numsNeighbor == 2 ) );
		}
	}
}

long serial_phase( Grid* g, unsigned int current_step )
{
	std::chrono::high_resolution_clock::time_point t1, t2;

	// Start - Serial Phase
	t1 = std::chrono::high_resolution_clock::now();

	// Swap the reading and writing matrixes.
	g->swap();

	// Every step we need to configure the border to properly respect the logic of the 2D toroidal grid
	g->copyBorder();

#if DEBUG
	// Print the result of the Game of Life iteration.
	std::string title = "ITERATION " + std::to_string( (long long) current_step ) + " -";
	g->print( title.c_str(), true );
#endif // DEBUG

	// End - Serial Phase
	t2 = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
}

void thread_body( int id, Grid* g, size_t start, size_t end, unsigned int iterations, bool vectorization, long* serial_time, SpinningBarrier* barrier )
{
#if DEBUG
	std::cout << "Thread " << id << " got range [" << start << "," << end << ")" << std::endl;
#endif // DEBUG

	int* numsNeighbours = NULL;

	if ( vectorization )
		numsNeighbours = (int*) _mm_malloc( VLEN * sizeof(int), 64 );

	for ( unsigned int k = 1; k <= iterations; k++ )
	{
		compute_generation( g, numsNeighbours, start, end, vectorization );
		// if we are computing the sequential or it is the last thread we compute the serial phase
		if ( barrier == NULL || barrier->is_last_thread() )
		{
			*serial_time = *serial_time + serial_phase( g, k );
			if ( barrier != NULL) barrier->notify_all();
		}
	}

	if ( vectorization )
		_mm_free( numsNeighbours );
}

void thread_version( unsigned int nw, Grid* g, size_t chunk, size_t rest, unsigned int iterations, bool vectorization )
{
	std::chrono::high_resolution_clock::time_point t1, t2;
	size_t start, stop = g->width();
	std::vector<std::thread> tid;
	long serial_time = 0;

	SpinningBarrier* barrier = new SpinningBarrier( nw );

	// Start - Creating Threads.
	t1 = std::chrono::high_resolution_clock::now();

	// Create and start the workers.
	for( int t = 0; t < nw; t++ )
	{
		start = stop;
		stop  = start + chunk + ((rest-- > 0) ? 1 : 0);
		tid.push_back( std::thread( thread_body, t, g, start, stop, iterations, vectorization, &serial_time, barrier) );
	}

	// End - Creating Threads.
	t2 = std::chrono::high_resolution_clock::now();
	printTime( t1, t2, "creating threads" );

	// Await the threads termination.
	for ( int t = 0; t < nw; t++ )
		tid[t].join();

	// Print the total time in order to compute the serial phase.
	printTime( serial_time, "serial phase" );

	// Print the total time in order to compute the barrier phase.
	printTime( barrier->get_time(), "barrier phase" );
}

void fastflow_version( unsigned int nw, Grid* g, size_t chunk, size_t rest, unsigned int iterations, bool vectorization )
{
	std::chrono::high_resolution_clock::time_point t1, t2;
	size_t start, stop = g->width();
	assert ( nw > 1 );

	// Start - Creating Threads.
	t1 = std::chrono::high_resolution_clock::now();

	// Create Farm.
	std::vector<std::unique_ptr<ff::ff_node>> workers;
	for ( int i = 0; i < nw; i++ )
	{
		start = stop;
		stop  = start + chunk + ((rest-- > 0) ? 1 : 0);
		workers.push_back( ff::make_unique<Worker>( i, g, start, stop, vectorization ) );
	}
	ff::ff_Farm<> farm( std::move( workers ) );

	// Removes the default collector.
	farm.remove_collector();

	// The scheduler gets in input the internal load-balancer.
	Master master( farm.getlb(), nw, g, iterations );
	farm.add_emitter( master );

	// Adds feedback channels between each worker and the scheduler.
	farm.wrap_around();

	if ( farm.run() < 0 )
		ff::error("Error: running farm.");

	// End - Creating Threads
	t2 = std::chrono::high_resolution_clock::now();
	printTime( t1, t2, "creating threads" );

	farm.wait();
}

int roundMultiple( int numToRound, int mul )
{
	assert ( mul != 0 );
	return ((numToRound + mul - 1) / mul) * mul;
}

void printTime( long duration, const char *msg )
{
#if MACHINE_TIME
	std::cout << "Time to " << msg << ": " << duration << std::endl;
#else
	char buffer[100];
	int choice = 0;
	const std::string time_strings[6] = { "microseconds", "milliseconds", "seconds", "minutes", "hours", "days" };
	const int divisor[6] = { 1, 1000, 1000, 60, 60, 24 };
	double time = duration;

	while ( (choice < 5) && (time >= 1000 ) )
	{
		choice++;
		time = time / divisor[choice];
	}
	sprintf( buffer, "%.2f", time );

	// Print time on screen
	std::cout << "Time to " << msg << ": " << buffer << " " << time_strings[choice] << "." << std::endl;
#endif // MACHINE_TIME
}

void printTime( std::chrono::high_resolution_clock::time_point t1, std::chrono::high_resolution_clock::time_point t2, const char *msg )
{
	printTime( (long) std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count(), msg );
}