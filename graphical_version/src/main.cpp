/**
 *	@file main.cpp
 *  @brief This file contains the main() function where the Game of Life is implemented.
 *  @author Federico Conte (draxent)
 *  
 *	Copyright 2015 Federico Conte
 *	https://github.com/Draxent/ConnectedComponents
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


#include <fco.hpp>
#include <iostream>
#include <cstdio>
#include <chrono>
#include <string>
#include <vector>
#include <thread>
#include "fco.hpp"
#include "program_options.hpp"
#include "matrix.hpp"
#include "barrier.hpp"

#if OPENCV
#include "matrixG.hpp"
#endif  // OPENCV

using namespace std::chrono;

/**
 * Print the elapsed time between \em{t1} and \em{t2} in microseconds, milliseconds or seconds depending on its value.
 * @param t1	starting time
 * @param t2	ending time.
 * @param msg	message to print toghether with the elapsed time.
 */
void printTime(high_resolution_clock::time_point t1, high_resolution_clock::time_point t2, const char *msg);

/**
* Function computed by the threads
* When the barrier is reached by all the threads, some "global"
* operations are performed on the shared boolean matrix <em>m</em>: print and swap.
* @param id			thread identifier
* @param barrier	shared object of the \ref Barrier class.
* @param m			shared object of the \ref Matrix class.
* @param start		row index of the starting working area.
* @param end		row index of the ending working area.
* @param steps		replication of data: number of steps.
*/
void compute( int id, Barrier* barrier, Matrix* m, int start, int end, int steps )
{
	std::cout << "Thread " << id << " got rows range [" << start << "," << end << ")" << std::endl;

	for ( int k = 1; k <= steps; k++ )
	{
		for ( int i = start; i < end; i++ )
		{
			for ( int j = 0; j < m->width(); j++ )
			{
				int numNeighbors = m->countNeighbors(i, j);

                // Alive Cell
                if ( m->get(i, j) )
                {
					if (numNeighbors < 2) m->set(i, j, false); // Cell dies since is alone
					else if (numNeighbors == 2 || numNeighbors == 3) m->set(i, j, true); // Cell stays alive
					else m->set(i, j, false); // Cell dies for starvation
				}
                // Empty Cell
                else
                {
					if (numNeighbors == 3) m->set(i, j, true); // A new cell is born
					else m->set(i, j, false);
                }
			}
		}

		barrier->apply( k );
	}
}

int main( int argc, char** argv )
{
	ProgramOptions po( argc, argv );

	// Print help message if the "--help" option is present.
	if ( po.exists( "--help" ) )
	{
		fco::FCO f1(fco::UNDERLINE);
		std::string num = f1.apply("NUM");
		std::string file = f1.apply("FILE");

		std::cout << "Usage: " << argv[0] << " [options] " << std::endl;
		std::cout << "Possible options:" << std::endl;
		std::cout << "\t -w " << num << ", --width " << num << " \t width of the matrix" << std::endl;
		std::cout << "\t\t\t\t The maximum width has to be equal to the number of pixels of the screen width ;" << std::endl;
		std::cout << "\t -h " << num << ", --height " << num << " \t height of the matrix" << std::endl;
		std::cout << "\t\t\t\t The maximum height has to be equal to the number of pixels of the screen height ;" << std::endl;
		std::cout << "\t -i " << file << ", --input " << file << " \t file where to retrieve the matrix" << std::endl;
		std::cout << "\t\t\t\t --width and --height options will be ignored ; " << std::endl;
		std::cout << "\t -s " << num << ", --steps " << num << " \t number of steps ;" << std::endl;
		std::cout << "\t -t " << num << ", --thread " << num << " \t number of threads ;" << std::endl;
#if OPENCV
		std::cout << "\t -g, --graphic \t\t activate the graphic mode ;" << std::endl;
		std::cout << "\t -o " << file << ", --output " << file << "  file where to save the generated video" << std::endl;
		std::cout << "\t\t\t\t if not present, the video will be only displayed and not saved ; " << std::endl;
#endif // OPENCV
		std::cout << "\t --help \t\t\t this help view ;" << std::endl;

		return 0;
	}

	// Retrieve all the options value
	int width = po.get_int( "-w", "--width", 100 );
	int height = po.get_int( "-h", "--height", 100 );
	int steps = po.get_int( "-s", "--steps", 100 );
	int nw = std::max( po.get_int( "-t", "--thread", std::thread::hardware_concurrency() ), 1 );
	assert ( width > 0 && height > 0 && steps > 0 );
	std::cout << "NUMBER OF CHOSEN THREADS = " << nw << " !!!" << std::endl;

	Matrix* m;
	high_resolution_clock::time_point t1, t2;

	// Create Matrix object depending on the program options.
#if OPENCV
	if ( po.exists( "-i", "--input" ) )
	{
		char* input_path = po.get( "-i", "--input" );
		m = po.exists( "-g", "--graphic" ) ? new MatrixG( input_path, po.get("-o", "--output"), steps ) : new Matrix( input_path );
		width = m->width();
		height = m->height();
	}
	else
		m = po.exists( "-g", "--graphic" ) ? new MatrixG( height, width, po.get("-o", "--output"), steps ) : new Matrix( height, width );
#else
	if ( po.exists( "-i", "--input" ) )
	{
		m = new Matrix( po.get( "-i", "--input" ) );
		width = m->width();
		height = m->height();
	}
	else
		m = new Matrix( height, width );
#endif  // OPENCV

	// Print initial configuration
	m->print();

	// Start the Game of Life
	t1 = high_resolution_clock::now();
	if (nw > 1)
	{
		Barrier* barrier = new Barrier( m, nw );
		std::vector<std::thread> tid;

		// Create and start the workers
		int chunk = height / nw, rest = height % nw, start = 0, stop = 0;
		for(int t = 0; t < nw; t++)
		{
			start = stop;
			stop  = start + chunk + ((rest-- > 0) ? 1 : 0);

			tid.push_back( std::thread( compute, t, barrier, m, start, stop, steps ) );
		}

		// Await the threads termination
		for (int t = 0; t < nw; t++)
			tid[t].join();
		std::cout << "Threads terminated!" << std::endl;
	}
	else
	{
		// Sequential version
		Barrier* barrier = new Barrier( m, 1 );
		compute( 1, barrier, m, 0, height, steps );
	}
	t2 = high_resolution_clock::now();
	printTime( t1, t2, "complete Game of Life" );

	return 0;
}

void printTime( high_resolution_clock::time_point t1, high_resolution_clock::time_point t2, const char *msg )
{
	char buffer[100];
	auto duration = duration_cast<microseconds>(t2 - t1).count();
	int choice = 0;
	float time = duration;
	const fco::FCO f1({ fco::BOLD, fco::FG_RED });
	const std::string time_strings[6] = { "microseconds", "milliseconds", "seconds", "minutes", "hours", "days" };
	const int divisor[6] = { 1, 1000, 1000, 60, 60, 24 };

	while ( (choice < 5) && (time >= 1000 ) )
	{
		choice++;
		time = time / divisor[choice];
	}
	sprintf (buffer, "%.2f", time);

	// Print time on screen
	std::cout << "Time to " << msg << ": " << f1.apply( buffer );
	std::cout << " " << time_strings[choice] << "." << std::endl;
}
