/**
 *	@file main.cpp
 *	@brief Contains the main() function where the Game of Life is implemented.
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
#include <cstdio>
#include <chrono>
#include <string>
#include <vector>
#include <thread>
#include <ff/farm.hpp>

#include <program_options.hpp>
#include <barrier.hpp>
#include <grid.hpp>
#include <functions.hpp>

int main( int argc, char** argv )
{
	std::chrono::high_resolution_clock::time_point t1, t2;
	ProgramOptions po( argc, argv );

	// Print help message if the "--help" option is present.
	if ( po.exists( "--help" ) )
	{
		std::cerr << "Usage: " << argv[0] << " [options] " << std::endl;
		std::cerr << "Possible options:" << std::endl;
		std::cerr << "\t -v,\t --vect \t activate the vectorization version ;" << std::endl;
		std::cerr << "\t -ff,\t --fastflow \t activate the FastFlow version ;" << std::endl;
		std::cerr << "\t -w NUM, --width NUM \t width of the matrix ;" << std::endl;
		std::cerr << "\t -h NUM, --height NUM \t height of the matrix ;" << std::endl;
		std::cerr << "\t -s NUM, --steps NUM \t number of steps ;" << std::endl;
		std::cerr << "\t -t NUM, --thread NUM \t number of threads ;" << std::endl;
		std::cerr << "\t --help \t\t this help view ;" << std::endl;
		return 1;
	}

	// Retrieve all the options value
	bool vectorization = po.exists( "-v", "--vect" );
	bool fastflow = po.exists( "-ff", "--fastflow" );
	size_t width = po.get_int( "-w", "--width", 1000 );
	size_t height = po.get_int( "-h", "--height", 1000 );
	size_t steps = po.get_int( "-s", "--steps", 100 );
	int nw = po.get_int( "-t", "--thread", 1 );
	assert ( nw > 0 && width > 0 && height > 0 && steps > 0 );
	std::cout << "Vectorization: " << ( vectorization ? "true" : "false" ) << ", FastFlow: " << ( fastflow ? "true" : "false" ) << ", ";
	std::cout << "Width: " << width << ", Height: " << height << ", Steps: " << steps << ", NumWorker: " << nw << "." << std::endl;
	
	// Start - Initialization Phase
	t1 = std::chrono::high_resolution_clock::now();
	
	// Create Matrix object.
	Grid* g = new Grid( height, width, vectorization );
	// Configure the border to properly respect the logic of the 2D toroidal grid
	g->copyBorder();
	// Create Barrier object.
	Barrier* barrier = new Barrier( g, nw, fastflow );

#if DEBUG
	// Print initial configuration
	g->print( "INPUT" );
#endif // DEBUG

	// End - Initialization Phase
	t2 = std::chrono::high_resolution_clock::now();	
	printTime( t1, t2, "initialization phase" );
	
	// Start - Game of Life
	t1 = std::chrono::high_resolution_clock::now();
	
	// Sequential version // nw == 0
	if ( nw == 1 )
		thread_body( 1, g, barrier, g->width(), g->size() - g->width(), steps, vectorization );
	else
	{
		// Set up some variables useful for the threads work
		size_t workingSize = g->size() - 2*g->width();
		// It is better that chunk is a multiple of VLEN in order to favor vectorization.
		size_t chunk = (vectorization) ? ( roundMultiple(workingSize / nw, VLEN) ) :  ( workingSize / nw );
		size_t r = workingSize - chunk*nw, rest = ( r > 0 ) ? r : 0;
	
#if DEBUG
		std::cout << "WorkingSize: " << workingSize << ", Chunk: " << chunk << ", Rest: " << rest << "." << std::endl;
#endif // DEBUG
	
		if ( fastflow )
			fastflow_version( nw, g, barrier, chunk, rest, steps, vectorization );
		else
			thread_version( nw, g, barrier, chunk, rest, steps, vectorization );
	}
	
#if DEBUG
	// Print final configuration
	g->print( "OUTPUT" );
#endif // DEBUG
	
	// Print the total time in order to compute the barrier phase.
	printTime( barrier->get_time(), "barrier phase" );
	
	// End - Game of Life
	t2 = std::chrono::high_resolution_clock::now();
	printTime( t1, t2, "complete Game of Life" );	

	return 0;
}