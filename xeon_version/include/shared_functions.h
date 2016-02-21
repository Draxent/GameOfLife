/**
 *	@file shared_functions.h
 *	@brief Header of some functions that are shared by all different development methodologies used in this application.
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

#ifndef GAMEOFLIFE_USEFUL_FUNCTIONS_H
#define GAMEOFLIFE_USEFUL_FUNCTIONS_H

#include <iostream>
#include <cstdio>
#include <chrono>
#include <string>

#include "program_options.h"
#include "grid.h"
#if DEBUG
#include "matrix.h"
#endif // DEBUG

#define MAX_PRINTABLE_GRID 32

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
 * @param numNeighbours	array where to store the computed neighbours counting, used only when the function is vectorized.
 * @param start				row index of starting working area.
 * @param end				row index of ending working area.
 */
void compute_generation_vect( Grid* g, int* numNeighbours, size_t start, size_t end );

/**
 * Sequential version of GOL
 * @param g					the \see Grid object.
 * @param iterations		number of iterations.
 * @param vectorization		<code>true</code> if the code has to be vectorized.
 * @return	<code>true</code> if the result of GOL is correct, <code>false</code> otherwise.
 */
bool sequential_version( Grid* g, unsigned int iterations, bool vectorization );

/**
 * It is the phase that we decided to not parallelize.
 * This includes: swap(), copyBorder() and print().
 * So this phase is executed by the last thread that reached the barrier
 * at the end of the computation of a generation.
 * @param g						the \see Grid object.
 * @param current_iteration		current GOL iteration, needed during DEBUG.
 * @return	time needed to compute it, in microseconds.
 */
long end_generation( Grid* g, unsigned int current_iteration );

/**
 * Shows the program options if flag "--help" is present and
 * properly configure the variables: seed, width, height, iterations, nw.
 * @param argc	number of external arguments.
 * @param argv	array of external arguments.
 * @param vectorization, grain, width, height, seed, iterations, nw	variables to configure.
 * @return	<code>true</code> if no error has occurred, <code>false</code> otherwise.
 */
bool menu( int argc, char** argv, bool& vectorization, size_t& grain, size_t& width, size_t& height, unsigned int& seed, unsigned int& iterations, unsigned int& nw );

/**
 * Initialization Phase.
 * @param vectorization, width, height, seed	external variables.
 * @param g		the \see Grid object that we want to initialize.
 */
void initialization( bool vectorization, size_t width, size_t height, unsigned int seed, Grid*& g );

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

#endif //GAMEOFLIFE_USEFUL_FUNCTIONS_H
