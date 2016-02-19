/**
 *	@file worker_vect.h
 *	@brief Header of \see WorkerVect class.
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

#ifndef GAMEOFLIFE_WORKER_VECT_H
#define GAMEOFLIFE_WORKER_VECT_H

#include "worker.h"

// Vectorized version of \see Worker class.
class WorkerVect : public Worker
{
public:
	/**
	 * Initializes a new instance of the \see WorkerVect class.
	 * We create an array to store the results of the number of Neighbour counting.
	 * @param id			Worker identifier.
	 * @param g				shared object of the \see GridVect class
	 * @param start			row index of the starting working area.
	 * @param end			row index of the ending working area.
	 */
	WorkerVect( int id, GridVect* g, size_t start, size_t end );

	/**
	 * FastFlow method of the \see ff::ff_node_t.
	 * Call \see compute_generation function, i.e. it responds to the Master
	 * request computing a new generation on its portion of the \see Grid.
	 * @param task	"GO" message received from the \see Master.
	 * @return		"DONE" message to the \see Master.
	 */
	bool* svc( bool* task );

	/**
	 * Destructor of the \see WorkerVect class.
	 * Delete the additional array created during initialization.
	 */
	~WorkerVect();

private:
	int* numsNeighbors;
};

#endif //GAMEOFLIFE_WORKER_VECT_H
