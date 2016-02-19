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

#include "../include/grid.h"
#include "../include/grid_vect.h"
#include "../include/shared_functions.h"
#if DEBUG
#include "../include/matrix.h"
#endif // DEBUG

int main( int argc, char** argv )
{
	std::chrono::high_resolution_clock::time_point t1, t2;
	bool vectorization;
	size_t width, height, chunk, rest, end;
	unsigned int seed, iterations, nw;
	Grid* g;
	// Configure the variables depending on the program options.
	if ( !menu( argc, argv, vectorization, width, height, seed, iterations, nw ) )
		return 1;
	initialization( vectorization, width, height, seed, iterations, nw, g, chunk, rest, end );

	// If nw is equal to zero, we have computed the result in the initialization function, so we can exit.
	if ( nw == 0 )
		return 0;

#if DEBUG
	// Initialize the matrix that we will used as verifier.
	Matrix* verifier = new Matrix( g );
#endif // DEBUG

	// Start - Game of Life & Start Creating Threads
	t1 = std::chrono::high_resolution_clock::now();

	size_t* starts = new size_t[nw + 1];
	size_t* stops = new size_t[nw + 1];
	starts[0] = 0;
	stops[0] = g->width() + 1;
	long serial_time = 0;

	// Calculate real number of threads and the working chunk of each thread.
	int n;
	for( n = 1; (n <= nw) && (stops[n - 1] < end); n++ )
	{
		starts[n] = stops[n - 1];
		stops[n] = starts[n] + chunk + ((rest-- > 0) ? 1 : 0);
		stops[n] = ( stops[n] > end ) ? end : stops[n];
	}
	nw = n - 1;

	// Create the Barrier passing the real number of threads that we are going to use.
	SpinningBarrier* barrier = new SpinningBarrier( nw );

	// Create and start the workers.
	std::vector<std::thread> tid;
	for( int t = 0; t < nw; t++ )
	{
		// The working size has to be significant in order to vectorized the thread_body function.
		if ( vectorization && ( stops[t + 1] - starts[t + 1] >= VLEN ) )
			tid.push_back( std::thread( thread_body_vect, t, (GridVect*) g, starts[t + 1], stops[t + 1], iterations, &serial_time, barrier ) );
		else
			tid.push_back( std::thread( thread_body, t, g, starts[t + 1], stops[t + 1], iterations, &serial_time, barrier ) );
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