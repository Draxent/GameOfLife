/**
 *	@file functions.hpp
 *	@brief Header of functions used in main.cpp file.
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

#ifndef INCLUDE_FUNCTIONS_HPP_
#define INCLUDE_FUNCTIONS_HPP_

#include <iostream>
#include <cstdio>
#include <chrono>
#include <string>
#include <vector>
#include <thread>
#include <ff/farm.hpp>

#include <barrier.hpp>
#include <grid.hpp>
#include <master.hpp>
#include <worker.hpp>

/**
* Compute a generation of Game of Life.
* 
* Rules for cells evolution are the following:
* IF ( cell is alive )
* 		IF ( #Neighbours < 2 )	 Cell dies due to under-population
* 		ELSE ( #Neighbours == 2 || #Neighbours == 3 )	Cell stays alive
* 		ELSE	Cell dies for starvation
* ELSE
* 		IF ( #Neighbours == 3 )	 A new cell is born by reproduction of one of the neighbour
* 		ELSE	Box remains empty
* 
* These rules can be re-writed as following:
* 	Box ← (( #Neighbours == 3 ) OR ( Cell is alive AND #Neighbours == 2 ))
* where the targeting box will contains or not a cell depending on the condition value.
* 
* @param g				shared object of \see Grid class.
* @param numsNeighbours	array where to store the computed neighbours counting, used only when the function is vectorized.
* @param start			row index of starting working area.
* @param end			row index of ending working area.
* @param vectorization	<code>true</code> if function has to be vectoriazed.
*/
void compute_generation( Grid* g, int* numsNeighbours, size_t start, size_t end, bool vectorization );

/**
* Function computed by threads.
* Starting from the initial Grid configuration, it computes a new generation for each step required.
* After each step, threads are synchronized with a barrier.
* When barrier is reached by all threads, some "global"
* operations are performed on the shared Grid <em>g</em>: swap, copyBorder, print.
* @param id				thread identifier
* @param g				shared object of \see Grid class.
* @param barrier		shared object of \see Barrier class.
* @param start			row index of starting working area.
* @param end			row index of ending working area.
* @param steps			number of steps.
* @param vectorization	<code>true</code> if the function has to be vectoriazed.
*/
void thread_body( int id, Grid* g, Barrier* barrier, size_t start, size_t end, size_t steps, bool vectorization );

/**
* Execute Thread Version.
* @param nw				number of threads.
* @param g				shared object of \see Grid class.
* @param barrier		shared object of \see Barrier class.
* @param chunk			chunk size given to each thread.
* @param rest			increment chunk size of a unit until is grater than zero.
* @param steps			number of steps.
* @param vectorization	<code>true</code> if the code has to be vectoriazed.
*/
void thread_version( int nw, Grid* g, Barrier* barrier, size_t chunk, size_t rest, int steps, bool vectorization );

/**
* Execute FastFlow Version.
* @param nw				number of Workers.
* @param g				shared object of \see Grid class.
* @param barrier		shared object of \see Barrier class.
* @param chunk			chunk size given to each Worker.
* @param rest			increment chunk size of a unit until is grater than zero.
* @param steps			number of steps.
* @param vectorization	<code>true</code> if code has to be vectoriazed.
*/
void fastflow_version( int nw, Grid* g, Barrier* barrier, size_t chunk, size_t rest, int steps, bool vectorization );

/**
 * Round up a number <em>numToRound</em> to a value that is multiple of <em>mul</em>.
 * @param numToRound	number to round up.
 * @param mul			<em>numToRound</em> has to be multiple of this number.
 * @return				the number rounded up.
 */
int roundMultiple( int numToRound, int mul );

/**
 * Print the elapsed time in appropriate unit depending on its value or in microseconds if MACHINE_TIME flag is on.
 * @param duration	elapsed time
 * @param msg	message to print together with the elapsed time.
 */
void printTime( unsigned long duration, const char *msg );

/**
 * Print the elapsed time between \em{t1} and \em{t2} in appropriate unit depending on its value or in microseconds if MACHINE_TIME flag is on.
 * @param t1	starting time
 * @param t2	ending time.
 * @param msg	message to print together with the elapsed time.
 */
void printTime( std::chrono::high_resolution_clock::time_point t1, std::chrono::high_resolution_clock::time_point t2, const char *msg );

#endif /* INCLUDE_FUNCTIONS_HPP_ */