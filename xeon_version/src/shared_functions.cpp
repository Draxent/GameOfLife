/**
 *	@file shared_functions.cpp
 *  @brief Implementation of some functions that are shared by all different development methodologies used in this application.
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

#include "../include/shared_functions.h"

bool sequential_version( Grid* g, unsigned int iterations, bool vectorization )
{
	std::chrono::high_resolution_clock::time_point t1, t2;

#if DEBUG
	// Initialize the matrix that we will used as verifier.
	Matrix* verifier = new Matrix( g );
#endif // DEBUG

	// Start - Game of Life
	t1 = std::chrono::high_resolution_clock::now();

	long copyborder_time = 0;
	thread_body( 0, g, g->width() + 1, g->size() - g->width() - 1, iterations, vectorization, &copyborder_time );

	// Print the total time in order to compute  the end_generation functions.
	printTime( copyborder_time, "copy border" );

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
		return false;
	}
#endif // DEBUG

	// End - Game of Life
	t2 = std::chrono::high_resolution_clock::now();
	printTime( t1, t2, "complete Game of Life" );
	return true;
}

long end_generation( Grid* g, unsigned int current_iteration )
{
	std::chrono::high_resolution_clock::time_point t1, t2;

	// Start - End Generation
	t1 = std::chrono::high_resolution_clock::now();

	// Swap the reading and writing matrixes.
	g->swap();

	// Every step we need to configure the border to properly respect the logic of the 2D toroidal grid
	g->copyBorder();

#if DEBUG
	// Print only small Grid
	if ( g->width() <= MAX_PRINTABLE_GRID && g->height() <= MAX_PRINTABLE_GRID )
	{
		// Print the result of the Game of Life iteration.
		std::string title = "ITERATION " + std::to_string( (long long) current_iteration ) + " -";
		g->print( title.c_str(), true );
	}
#endif // DEBUG

	// End - End Generation
	t2 = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
}

bool menu( int argc, char** argv, bool& vectorization, size_t& width, size_t& height, unsigned int& seed, unsigned int& iterations, unsigned int& nw )
{
	ProgramOptions po( argc, argv );

	// Print help message if the "--help" option is present.
	if ( po.exists( "--help" ) )
	{
		std::cerr << "Usage: " << argv[0] << " [options] " << std::endl;
		std::cerr << "Possible options:" << std::endl;
		std::cerr << "\t -v,\t --vect \t activate the vectorization version ;" << std::endl;
		std::cerr << "\t -w NUM, --width NUM \t grid width ;" << std::endl;
		std::cerr << "\t -h NUM, --height NUM \t grid height ;" << std::endl;
		std::cerr << "\t -s NUM, --seed NUM \t seed used to initialize the grid ( zero for timestamp seed ) ;" << std::endl;
		std::cerr << "\t -i NUM, --iterations NUM \t number of iterations ;" << std::endl;
		std::cerr << "\t -t NUM, --thread NUM \t number of threads ( zero for the sequential version ) ;" << std::endl;
		std::cerr << "\t --help \t\t this help view ;" << std::endl;
		return false;
	}

	// Retrieve all the options value
	vectorization = po.exists( "-v", "--vect" );
	width = (size_t) po.get_number( "-w", "--width", 1000 );
	height = (size_t) po.get_number( "-h", "--height", 1000 );
	seed = (unsigned int) po.get_number( "-s", "--seed", 0 );
	iterations = (unsigned int) po.get_number( "-i", "--iterations", 100 );
	nw = (unsigned int) po.get_number( "-t", "--thread", 0 );
	assert ( nw >= 0 && width > 0 && height > 0 && iterations > 0 );
	std::cout << "Vectorization: " << ( vectorization ? "true" : "false" ) << ", ";
	std::cout << "Width: " << width << ", Height: " << height << ", Seed: " << seed;
	std::cout << ", Iterations: " << iterations << ", NumWorker: " << nw << "." << std::endl;
	return true;
}

bool initialization( bool vectorization, size_t width, size_t height, unsigned int seed, unsigned int iterations, unsigned int nw, Grid*& g, size_t& chunk, size_t& rest, size_t& end )
{
	std::chrono::high_resolution_clock::time_point t1, t2;
	// Start - Initialization Phase
	t1 = std::chrono::high_resolution_clock::now();

	// Create and initialize the Grid object.
	g = new Grid( height, width );
	if ( vectorization ) g->init_vect( seed );
	else g->init( seed );
	// Configure the border to properly respect the logic of the 2D toroidal grid
	g->copyBorder();

#if DEBUG
	// Print only small Grid
	if ( g->width() <= MAX_PRINTABLE_GRID && g->height() <= MAX_PRINTABLE_GRID )
	{
		// Print initial configuration.
		g->print( "INPUT" );
		// Print iteration zero.
		g->print( "ITERATION 0 -", true );
	}
#endif // DEBUG

	// End - Initialization Phase
	t2 = std::chrono::high_resolution_clock::now();
	printTime( t1, t2, "initialization phase" );

	// Sequential version
	if ( nw == 0 )
		return sequential_version( g, iterations, vectorization );

	// Set up some variables useful for the threads work
	size_t workingSize = g->size() - 2*g->width();
	// It is better that chunk is a multiple of VLEN in order to favor vectorization.
	chunk = (vectorization) ? ( roundMultiple(workingSize / nw, VLEN) ) :  ( workingSize / nw );
	chunk = ( chunk < VLEN ) ? VLEN : chunk;
	long r = workingSize - chunk*nw;
	rest = ( r > 0 ) ? r : 0;
	end = g->size() - g->width() - 1;

#if DEBUG
	std::cout << "WorkingSize: " << workingSize << ", Chunk: " << chunk << ", Rest: " << rest << "." << std::endl;
#endif // DEBUG

	return true;
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