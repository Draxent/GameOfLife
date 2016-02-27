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

void compute_generation( Grid* g, size_t start, size_t end )
{
	size_t pos_top = start - g->width(), pos_bottom = start + g->width();

	for ( size_t pos = start; pos < end; pos++, pos_top++, pos_bottom++ )
	{
		// Calculate #Neighbours.
		int numNeighbor = g->countNeighbours( pos, pos_top, pos_bottom );
		// Box ← (( #Neighbours == 3 ) OR ( Cell is alive AND #Neighbours == 2 )).
		g->Write[pos] = ( numNeighbor == 3 || ( g->Read[pos] && numNeighbor == 2 ) );
	}
}

#if VECTORIZATION
void compute_generation_vect( Grid* g, int* numNeighbours, size_t start, size_t end )
{
	size_t index = start, index_top = start - g->width(), index_bottom = start + g->width();
	for ( ; index + VLEN < end; index += VLEN, index_top += VLEN, index_bottom += VLEN )
	{
		// Save the computation of the neighbours counting into the numNeighbours array.
		numNeighbours[0:VLEN] = g->countNeighbours( index, index_top, index_bottom, __sec_implicit_index(0) );
		// Box ← (( #Neighbours == 3 ) OR ( Cell is alive AND #Neighbours == 2 )) in vector notations.
		g->Write[index:VLEN] = ( numNeighbours[0:VLEN] == 3 || ( g->Read[index:VLEN] && numNeighbours[0:VLEN] == 2 ) );
	}
	// Compute normally the last piece that does not fill the numNeighbours array.
	for ( ; index < end; index++, index_top++, index_bottom++ )
	{
		// Calculate #Neighbours.
		int numNeighbor = g->countNeighbours( index, index_top, index_bottom );
		// Box ← (( #Neighbours == 3 ) OR ( Cell is alive AND #Neighbours == 2 )).
		g->Write[index] = ( numNeighbor == 3 || ( g->Read[index] && numNeighbor == 2 ) );
	}
}
#endif // VECTORIZATION

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
	size_t start = g->width() + 1, end = g->size() - g->width() - 1;
	int* numNeighbours = NULL;
	if ( vectorization )
		numNeighbours = new int[VLEN];

	for ( unsigned int k = 1; k <= iterations; k++ )
	{
#if VECTORIZATION
		if ( vectorization )
			compute_generation_vect( g, numNeighbours, start, end );
		else
			compute_generation( g, start, end );
#else
		compute_generation( g, start, end );
#endif // VECTORIZATION
		copyborder_time = copyborder_time + end_generation( g, k );
	}

	if ( vectorization )
		delete[] numNeighbours;

#if TAKE_ALL_TIME
	// Print the total time in order to compute  the end_generation functions.
	printTime( copyborder_time, "copy border" );
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
		return false;
	}
#endif // DEBUG

	return true;
}

long end_generation( Grid* g, unsigned int current_iteration )
{
#if TAKE_ALL_TIME
	std::chrono::high_resolution_clock::time_point t1, t2;
	// Start - End Generation
	t1 = std::chrono::high_resolution_clock::now();
#endif // TAKE_ALL_TIME

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

#if TAKE_ALL_TIME
	// End - End Generation
	t2 = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
#else
	return  0;
#endif // TAKE_ALL_TIME
}

bool menu( int argc, char** argv, bool& vectorization, unsigned int& num_tasks, size_t& width, size_t& height, unsigned int& seed, unsigned int& iterations, unsigned int& nw )
{
	ProgramOptions po( argc, argv );

	// Print help message if the "--help" option is present.
	if ( po.exists( "--help" ) )
	{
		std::cerr << "Usage: " << argv[0] << " [options] " << std::endl;
		std::cerr << "Possible options:" << std::endl;
#if VECTORIZATION
		std::cerr << "\t -v,\t --vect \t activate the vectorization version ;" << std::endl;
#endif // VECTORIZATION
		std::cerr << "\t -w NUM, --width NUM \t grid width ;" << std::endl;
		std::cerr << "\t -h NUM, --height NUM \t grid height ;" << std::endl;
		std::cerr << "\t -s NUM, --seed NUM \t seed used to initialize the grid ( zero for timestamp seed ) ;" << std::endl;
		std::cerr << "\t -i NUM, --iterations NUM \t number of iterations ;" << std::endl;
		std::cerr << "\t -t NUM, --thread NUM \t number of threads ( zero for the sequential version ) ;" << std::endl;
		std::cerr << "\t -n NUM, --num_tasks NUM \t  number of tasks generated ;" << std::endl;
		std::cerr << "\t --help \t\t this help view ;" << std::endl;
		return false;
	}

	// Retrieve all the options value
	width = (size_t) po.get_number( "-w", "--width", 1000 );
	height = (size_t) po.get_number( "-h", "--height", 1000 );
	seed = (unsigned int) po.get_number( "-s", "--seed", 0 );
	iterations = (unsigned int) po.get_number( "-i", "--iterations", 100 );
	nw = (unsigned int) po.get_number( "-t", "--thread", 0 );
	num_tasks = (unsigned int) po.get_number( "-n", "--num_chunks", nw );
	// At least one task per Worker.
	assert ( num_tasks >= 0 && nw >= 0 && width > 0 && height > 0 && iterations > 0 );
#if VECTORIZATION
	vectorization = po.exists( "-v", "--vect" );
	std::cout << "Vectorization: " << ( vectorization ? "true" : "false" ) << ", ";
#else
	vectorization = false;
#endif // VECTORIZATION
	std::cout << "Width: " << width << ", Height: " << height << ", Seed: " << seed;
	std::cout << ", #Iterations: " << iterations << ", #Workers: " << nw << ", #Tasks: " << num_tasks << "." << std::endl;
	return true;
}

void initialization( bool vectorization, size_t width, size_t height, unsigned int seed, Grid*& g )
{
	std::chrono::high_resolution_clock::time_point t1, t2;
	// Start - Initialization Phase
	t1 = std::chrono::high_resolution_clock::now();

	// Create and initialize the Grid object.
	g = new Grid( height, width );
	g->init( seed );
#if VECTORIZATION
	if ( vectorization ) g->init_vect( seed );
	else g->init( seed );
#else
	g->init( seed );
#endif // VECTORIZATION
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
}

void setup_working_variable( Grid* g, unsigned int& num_tasks, unsigned int& nw, size_t& start, size_t*& chunks )
{
	size_t workingSize = g->size() - 2*g->width() - 2;
	start = g->width() + 1;
	// The minimum percentage has to guarantee a task of at least MIN_BLOCK_SIZE size.
	double min_perc = MIN_BLOCK_SIZE / (double) workingSize;
	// Calculate the maximum number of tasks given the minimum percentage calculation.
	unsigned long max_num_tasks = (unsigned long) ceil( 1 / min_perc );
	// If the workingSize is small, we do not need so many tasks.
	num_tasks = ( max_num_tasks < num_tasks ) ? ((int) max_num_tasks) : num_tasks;
	// Adjust the number of Workers in order to have at least one task per Worker.
	nw = ( num_tasks < nw ) ? num_tasks : nw;

	// Calculate the chunk size of each task, with a decreasing cubic function which
	// summation is equal to 100% of the workingSize.
	chunks = new size_t[num_tasks];
	// This percentage amount is fixed, since each task has at least min_perc of workingSize.
	double fix_perc = min_perc * num_tasks;
	// Compute the summation.
	unsigned long long summation = 1;
	for ( int i = 2; i <= num_tasks; i++ ) summation += pow3(i);
	// Calculate the variable that define our cubic function.
	double multiplier = (1 - fix_perc) / (double) summation;
	// Since we are working with integer numbers, we can have a rest.
	size_t rest = workingSize;
	// Fill the chunks array with the calculated chunk size for each task.
	for ( int i = 0; i < num_tasks; i++ )
	{
		double percentage = min_perc + multiplier * pow3(num_tasks - i);
		chunks[i] = percentage * workingSize;
		rest -= chunks[i];
	}
	// Assign the rest to the first task.
	chunks[0] += rest;

#if DEBUG
	std::cout << "Working Size: " << workingSize << ", #Workers: " << nw << ", #Tasks : " << num_tasks << std::endl;
	std::cout << "CHUNKS = { " << chunks[0];
	for ( int i = 1; i < num_tasks; i++ )
		std::cout << ", " << chunks[i];
	std::cout << " }" << std::endl;
#endif // DEBUG
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