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
#include <cmath>

#include "../include/grid.h"
#include "../include/shared_functions.h"
#if DEBUG
#include "../include/matrix.h"
#endif // DEBUG

void thread_body( int id, Grid* g, bool vectorization, size_t* start, size_t* end, std::atomic<bool>* terminate, std::atomic<bool>* busy );
int find_first_thread_free( std::atomic<bool>* busy, unsigned int nw );
long barrier( std::atomic<bool>* busy, unsigned int nw );

int main( int argc, char** argv )
{
	std::chrono::high_resolution_clock::time_point t1, t2;
	bool vectorization;
	size_t grain, width, height;
	unsigned int seed, iterations, nw;
	Grid* g;
	// Configure the variables depending on the program options.
	if ( !menu( argc, argv, vectorization, grain, width, height, seed, iterations, nw ) )
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

	// Set up some variables useful for the threads work
	size_t workingSize = g->size() - 2*g->width() - 2;
	size_t chunk_size = ( grain == 0 ) ? (workingSize / nw) : grain;
	chunk_size = ( chunk_size < VLEN ) ? VLEN : chunk_size;
	size_t start = g->width() + 1, end = g->size() - g->width() - 1;
	unsigned long num_tasks = ceil( workingSize / chunk_size );
	// If the ideal number of workers is less then the required number, update nw (i.e. we do not need so many workers )
	nw = ( num_tasks < nw ) ? ((unsigned int) num_tasks) : nw;

#if DEBUG
	std::cout << "Chunk Size: " << chunk_size << ", Start: " << start << ", End: " << end << std::endl;
#endif // DEBUG

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
		size_t start_chunk, end_chunk = start;

		while ( end_chunk < end )
		{
			int t = find_first_thread_free( busy, nw );
			start_chunk = end_chunk;
			end_chunk = std::min( start_chunk + chunk_size, end );
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

	// Print the total time in order to compute the end_generation functions.
	printTime( copyborder_time, "copy border" );

	// Print the total time in order to compute the barrier phase.
	printTime( barrier_time, "barrier phase" );

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
			// Execute the job on the assigned chunk.
			if ( vectorization )
				compute_generation_vect( g, numNeighbours, *start, *end );
			else
				compute_generation( g, *start, *end );

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
	std::chrono::high_resolution_clock::time_point t1, t2;

	// Start - Barrier phase.
	t1 = std::chrono::high_resolution_clock::now();

	// Scan all threads sequentially and wait that all finish their jobs.
	for ( int i = 0; i < nw; i++ )
	{
		// Wait until the thread has not finish its job, i.e. is not busy anymore.
		while ( busy[i].load() );
	}

	// End - Barrier phase.
	t2 = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
}