/**
 *	@file ff.cpp
 *	@brief Contains the main() function where Game of Life is implemented and parallelized with FastFlow framework.
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
#include <cmath>
#include <ff/farm.hpp>

#include "../include/shared_functions.h"
#include "../include/master.h"
#include "../include/worker.h"
#if DEBUG
#include "../include/matrix.h"
#endif // DEBUG

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
	nw = ( num_tasks < nw ) ? ((int) num_tasks) : nw;

#if DEBUG
	std::cout << "Chunk Size: " << chunk_size << ", Start: " << start << ", End: " << end << std::endl;
#endif // DEBUG

	// Create Farm.
	std::vector<std::unique_ptr<ff::ff_node>> workers;
	for ( int t = 0; t < nw; t++ )
		workers.push_back( ff::make_unique<Worker>( t, g, vectorization ) );
	// Create the Farm.
	ff::ff_Farm<> farm( std::move( workers ) );

	// Removes the default collector.
	farm.remove_collector();

	// The scheduler gets in input the internal load-balancer.
	Master master( farm.getlb(), g, iterations, start, end, chunk_size, num_tasks );
	farm.add_emitter( master );

	// Adds feedback channels between each worker and the scheduler.
	farm.wrap_around();

	if ( farm.run() < 0 )
		ff::error("Error: running farm.");

	// End - Creating Threads
	t2 = std::chrono::high_resolution_clock::now();
	printTime( t1, t2, "creating threads" );

	farm.wait();

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

	// End - Game of Life
	t2 = std::chrono::high_resolution_clock::now();
	printTime( t1, t2, "complete Game of Life" );
	return 0;
}
