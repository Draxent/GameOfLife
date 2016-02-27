/**
 *	@file thread.cpp
 *	@brief Contains the main() function where Game of Life is implemented and parallelized with low level threading mechanisms.
 *	@author Federico Conte (draxent)
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

#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <atomic>

#include "../include/grid.h"
#include "../include/shared_functions.h"
#if DEBUG
#include "../include/matrix.h"
#endif // DEBUG

#define LOOSE_SOME_TIME 1000

/**
 * Function executed by the thread.
 * @param id				thread identifier
 * @param g					shared object of \see Grid class.
 * @param vectorization		<code>true</code> if the function has to be parallelized.
 * @param start				location address where main() stores index of starting working area.
 * @param end				location address where main() stores index of ending working area.
 * @param terminate			shared atomic variable used to control the thread termination.
 * @param busy				shared atomic variable used to check if the thread is still working or assign to it work to do.
 */
void thread_body( int id, Grid* g, bool vectorization, size_t* start, size_t* end, std::atomic<bool>* terminate, std::atomic<bool>* busy );

/**
 * Find the first thread free ( not busy ).
 * We remind that each thread has its own element of the busy array, shared only with main().
 * It scan this busy array, looking for the first thread free, until it finds one.
 * @param busy		array of atomic variables.
 * @param nw		number of threads.
 */
int find_first_thread_free( std::atomic<bool>* busy, unsigned int nw );

/**
 * Wait until all threads have finish their jobs.
 * It wait the termination of the first thread, than wait for the second, and so on.
 * @param busy		array of atomic variables.
 * @param nw		number of threads.
 */
long barrier( std::atomic<bool>* busy, unsigned int nw );

int main( int argc, char** argv )
{
	std::chrono::high_resolution_clock::time_point t1, t2;
	bool vectorization;
	size_t width, height;
	unsigned int seed, iterations, nw, num_tasks;
	Grid* g;
	// Configure the variables depending on the program options.
	if ( !menu( argc, argv, vectorization, num_tasks, width, height, seed, iterations, nw ) )
		return 1;
	initialization( vectorization, width, height, seed, g );

	// Sequential version
	if ( nw == 0 )
		return ( sequential_version( g, iterations, vectorization ) ? 0 : 1 );

#if DEBUG
	// Initialize the matrix that we will used as verifier.
	Matrix* verifier = new Matrix( g );
#endif // DEBUG

	// Start - Game of Life & Start Creating Threads
	t1 = std::chrono::high_resolution_clock::now();

	size_t start;
	size_t* chunks;
	setup_working_variable( g, num_tasks, nw, start, chunks );

	// If busy[i] is true, means that the i-th thread has received a task or is still computing its task.
	std::atomic<bool>* busy = new std::atomic<bool>[nw];
	// If true, all thread has to terminate their execution.
	std::atomic<bool> terminate;
	terminate.store( false );
	// Index of the starting and ending working area.
	// The main() changes these values in order to control the work of the threads.
	size_t* starts = new size_t[nw];
	size_t* ends = new size_t[nw];

	// Create and start the workers.
	std::vector<std::thread> tid;
	for( int t = 0; t < nw; t++ )
	{
		busy[t].store( false );
		tid.push_back( std::thread( thread_body, t, g, vectorization, &starts[t], &ends[t], &terminate, &busy[t] ) );
	}

	// End - Creating Threads.
	t2 = std::chrono::high_resolution_clock::now();
	printTime( t1, t2, "creating threads" );

	// Compute GOL
	long copyborder_time = 0, barrier_time = 0;
	for ( unsigned int k = 1; k <= iterations; k++ )
	{
		unsigned int counter_sent_tasks = 0;
		size_t start_chunk, end_chunk = start;

		while ( counter_sent_tasks < num_tasks )
		{
			int t = find_first_thread_free( busy, nw );
			start_chunk = end_chunk;
			end_chunk = start_chunk + chunks[counter_sent_tasks];
			counter_sent_tasks++;
			starts[t] = start_chunk;
			ends[t] = end_chunk;
			busy[t].store( true );
		}

		barrier_time += barrier( busy, nw );
		copyborder_time += end_generation( g, k );
	}

	// Terminate all threads.
	for ( int t = 0; t < nw; t++ )
		terminate.store( true );

	// Await the threads termination.
	for ( int t = 0; t < nw; t++ )
		tid[t].join();
	delete[] chunks;

#if TAKE_ALL_TIME
	// Print the total time in order to compute the end_generation functions.
	printTime( copyborder_time, "copy border" );

	// Print the total time in order to compute the barrier phase.
	printTime( barrier_time, "barrier phase" );
#endif // TAKE_ALL_TIME

	// End - Game of Life
	t2 = std::chrono::high_resolution_clock::now();
	printTime( t1, t2, "complete Game of Life" );

#if DEBUG
	// Print only small Grid
	if ( g->width() <= MAX_PRINTABLE_GRID && g->height() <= MAX_PRINTABLE_GRID )
	{
		// Print final configuration
		g->print( "OUTPUT" );
	}

	// Check if the output is correct.
	verifier->GOL( iterations );
	if ( verifier->equal() ) std::cout << "TEST OK !!! " << std::endl;
	else
	{
		std::cout << "Error: the verifier obtain this following different value for the GOL computation:" << std::endl;
		verifier->print();
		return 1;
	}
#endif // DEBUG
	return 0;
}

void thread_body( int id, Grid* g, bool vectorization, size_t* start, size_t* end, std::atomic<bool>* terminate, std::atomic<bool>* busy )
{
	int* numNeighbours = NULL;
	if ( vectorization )
		numNeighbours = new int[VLEN];

	// Loop until the master does not say that it can terminate.
	while ( !terminate->load() )
	{
		// Enters in a busy-looping until there is not work to do or has to terminate.
		while ( !busy->load() && !terminate->load() );

		// If does not have to terminate, it executes its job.
		if ( !terminate->load() )
		{
#if VECTORIZATION
			// Execute the job on the assigned chunk.
			if ( vectorization )
				compute_generation_vect( g, numNeighbours, *start, *end );
			else
				compute_generation( g, *start, *end );
#else
			compute_generation( g, *start, *end );
#endif // VECTORIZATION

			// Signal that now is free.
			busy->store( false );
		}
	}

	if ( vectorization )
		delete[] numNeighbours;
}

int find_first_thread_free( std::atomic<bool>* busy, unsigned int nw )
{
	int found = -1;
	// Repeat looking to a free thread until it founds one.
	while ( found == -1 )
	{
		// Scan all threads sequentially.
		for ( int i = 0; i < nw; i++ )
		{
			// We have finish when we found a not busy thread.
			if ( !busy[i].load() )
			{
				found = i;
				break;
			}
		}
	}
	return found;
}

long barrier( std::atomic<bool>* busy, unsigned int nw )
{
#if TAKE_ALL_TIME
	std::chrono::high_resolution_clock::time_point t1, t2;
	// Start - Barrier phase.
	t1 = std::chrono::high_resolution_clock::now();
#endif // TAKE_ALL_TIME

	// Scan all threads sequentially and wait that all finish their jobs.
	for ( int i = 0; i < nw; i++ )
	{
		// Wait until the thread has not finish its job, i.e. is not busy anymore.
		while ( busy[i].load() )
		{
			// Loose some time before retrying.
			for ( volatile unsigned int j = 0; j < LOOSE_SOME_TIME; j++ );
		}
	}

#if TAKE_ALL_TIME
	// End - Barrier phase.
	t2 = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
#else
	return 0;
#endif // TAKE_ALL_TIME
}