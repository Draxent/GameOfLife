/**
 *	@file functions.h
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

#ifndef GAMEOFLIFE_FUNCTIONS_H
#define GAMEOFLIFE_FUNCTIONS_H

#include <iostream>
#include <cstdio>
#include <chrono>
#include <string>
#include <vector>
#include <thread>
#include <mm_malloc.h>
#include <ff/farm.hpp>

#include "spinning_barrier.h"
#include "grid.h"
#include "master.h"
#include "worker.h"



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
 * These rules can be re-wrote as following:
 * 	Box ← (( #Neighbours == 3 ) OR ( Cell is alive AND #Neighbours == 2 ))
 * where the targeting box will contains or not a cell depending on the condition value.
 *
 * @param g					shared object of \see Grid class.
 * @param numsNeighbours	array where to store the computed neighbours counting, used only when the function is vectorized.
 * @param start				row index of starting working area.
 * @param end				row index of ending working area.
 * @param vectorization		<code>true</code> if function has to be vectorized.
 */
void compute_generation( Grid* g, int* numsNeighbours, size_t start, size_t end, bool vectorization );


/**
 * It is the phase that we decided to not parallelize.
 * This includes: swap(), copyBorder(), print().
 * So this phase is executed by the last thread that reached the barrier
 * at the end of the computation of a generation.
 * @param g					the \see Grid object.
 * @param current_step		current GOL step, needed during DEBUG.
 * @return					time needed to compute it, in microseconds.
 */
long serial_phase( Grid* g, unsigned int current_step );

/**
 * Function computed by threads.
 * Starting from the initial Grid configuration, it computes a new generation for each step required.
 * After each step, threads are synchronized with a barrier.
 * When barrier is reached by all threads, some "global"
 * operations are performed on the shared Grid <em>g</em>: swap, copyBorder, print.
 * @param id				thread identifier
 * @param g					shared object of \see Grid class.
 * @param start				row index of starting working area.
 * @param end				row index of ending working area.
 * @param iterations		number of iterations.
 * @param vectorization		<code>true</code> if the function has to be vectorized.
 * @param serial_time		address to the variable counting the time to compute the serial phase.
 * @param barrier			shared object of \see SpinningBarrier class.
 */
void thread_body( int id, Grid* g, size_t start, size_t end, unsigned int iterations, bool vectorization, long* serial_time, SpinningBarrier* barrier = NULL);

/**
 * Execute Thread Version.
 * @param nw				number of threads.
 * @param g					shared object of \see Grid class.
 * @param chunk				chunk size given to each thread.
 * @param rest				increment chunk size of a unit until is grater than zero.
 * @param iterations		number of iterations.
 * @param vectorization		<code>true</code> if the code has to be vectorized.
 */
void thread_version( unsigned int nw, Grid* g, size_t chunk, size_t rest, unsigned int iterations, bool vectorization );

/**
 * Execute FastFlow Version.
 * @param nw				number of Workers.
 * @param g					shared object of \see Grid class.
 * @param chunk				chunk size given to each Worker.
 * @param rest				increment chunk size of a unit until is grater than zero.
 * @param iterations		number of iterations.
 * @param vectorization		<code>true</code> if code has to be vectorized.
 */
void fastflow_version( unsigned int nw, Grid* g, size_t chunk, size_t rest, unsigned int iterations, bool vectorization );

/**
 * Round up a number <em>numToRound</em> to a value that is multiple of <em>mul</em>.
 * @param numToRound		number to round up.
 * @param mul				<em>numToRound</em> has to be multiple of this number.
 * @return					the number rounded up.
 */
int roundMultiple( int numToRound, int mul );

/**
 * Print the elapsed time in appropriate unit depending on its value or in microseconds if MACHINE_TIME flag is on.
 * @param duration	elapsed time
 * @param msg		message to print together with the elapsed time.
 */
void printTime( long duration, const char *msg );

/**
 * Print the elapsed time between \em{t1} and \em{t2} in appropriate unit depending on its value or in microseconds if MACHINE_TIME flag is on.
 * @param t1	starting time
 * @param t2	ending time.
 * @param msg	message to print together with the elapsed time.
 */
void printTime( std::chrono::high_resolution_clock::time_point t1, std::chrono::high_resolution_clock::time_point t2, const char *msg );

#endif //GAMEOFLIFE_FUNCTIONS_H