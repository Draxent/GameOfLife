/**
 *	@file worker.h
 *	@brief Header of \see Worker class.
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

#ifndef GAMEOFLIFE_WORKER_H
#define GAMEOFLIFE_WORKER_H

#include <ff/farm.hpp>
#include "grid.h"
#include "task.h"
#include "shared_functions.h"

/// This Worker computes GOL generations until \see Master command.
class Worker : public ff::ff_node_t<Task_t>
{
public:
	/**
	 * Initializes a new instance of the \see Worker class.
	 * If vectorization is <code>true</code>, we create an array
	 * to store the results of the number of Neighbour counting.
	 * @param id			Worker identifier.
	 * @param g				shared object of the \see Grid class
	 * @param vectorization	<code>true</code> if we want to execute the vectorized version.
	 */
	Worker( int id, Grid* g, bool vectorization);

	/**
	 * FastFlow method of the \see ff::ff_node_t.
	 * Call \see compute_generation function, i.e. it responds to the Master request computing a new generation on its portion of the \see Grid.
	 * @param task	"GO" message received from the \see Master.
	 * @return		"DONE" message to the \see Master.
	 */
	Task_t* svc( Task_t* task );

	/**
	 * Destructor of the \see Worker class.
	 * If vectorization is <code>true</code>, we delete the additional array created during initialization.
	 */
	~Worker();

private:
	int id;
	bool vectorization;
	Grid* g;
	int* numNeighbours;
};

#endif //GAMEOFLIFE_WORKER_H