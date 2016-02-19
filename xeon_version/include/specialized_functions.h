/**
 *	@file specialized_functions.h
 *	@brief Header of functions needed to development different methodologies.
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
#include <thread>
#if DEBUG
#include <mutex>
#endif // DEBUG

#include "program_options.h"
#include "shared_functions.h"
#include "grid.h"
#include "grid_vect.h"
#include "spinning_barrier.h"

/**
 * Compute a generation of Game of Life.
 * Rules for cells evolution are the following:
 * IF ( cell is alive )
 * 		IF ( #Neighbours < 2 )	 Cell dies due to under-population
 * 		ELSE ( #Neighbours == 2 || #Neighbours == 3 )	Cell stays alive
 * 		ELSE	Cell dies for starvation
 * ELSE
 * 		IF ( #Neighbours == 3 )	 A new cell is born by reproduction of one of the neighbour
 * 		ELSE	Box remains empty
 * These rules can be re-wrote as following:
 * 	Box ← (( #Neighbours == 3 ) OR ( Cell is alive AND #Neighbours == 2 ))
 * where the targeting box will contains or not a cell depending on the condition value.
 *
 * @param g					shared object of \see Grid class.
 * @param start				row index of starting working area.
 * @param end				row index of ending working area.
 */
void compute_generation( Grid* g, size_t start, size_t end );

/**
 * Vectorized version of \see compute_generation.
 * @param g					shared object of \see Grid class.
 * @param numsNeighbours	array where to store the computed neighbours counting, used only when the function is vectorized.
 * @param start				row index of starting working area.
 * @param end				row index of ending working area.
 */
void compute_generation_vect( GridVect* g, int* numsNeighbours, size_t start, size_t end );

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
 * @param serial_time		address to the variable counting the time to compute the serial phase.
 * @param barrier			shared object of \see SpinningBarrier class.
 */
void thread_body( int id, Grid* g, size_t start, size_t end, unsigned int iterations, long* serial_time, SpinningBarrier* barrier = NULL);

/**
 * Vectorized version of \see thread_body.
 * @param id				thread identifier
 * @param g					shared object of \see Grid class.
 * @param start				row index of starting working area.
 * @param end				row index of ending working area.
 * @param iterations		number of iterations.
 * @param serial_time		address to the variable counting the time to compute the serial phase.
 * @param barrier			shared object of \see SpinningBarrier class.
 */
void thread_body_vect( int id, GridVect* g, size_t start, size_t end, unsigned int iterations, long* serial_time, SpinningBarrier* barrier = NULL);

#endif //GAMEOFLIFE_FUNCTIONS_H