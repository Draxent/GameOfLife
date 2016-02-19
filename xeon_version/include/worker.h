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

#include <chrono>
#include <ff/farm.hpp>
#include "grid.h"
#include "specialized_functions.h"

/// This Worker computes GOL generations until \see Master command.
class Worker : public ff::ff_node_t<bool>
{
public:
	/**
	 * Initializes a new instance of the \see Worker class.
	 * @param id			Worker identifier.
	 * @param g				shared object of the \see Grid class
	 * @param start			row index of the starting working area.
	 * @param end			row index of the ending working area.
	 */
	Worker( int id, Grid* g, size_t start, size_t end);

	/**
	 * FastFlow method of the \see ff::ff_node_t.
	 * Call \see compute_generation function, i.e. it responds to the Master request computing a new generation on its portion of the \see Grid.
	 * @param task	"GO" message received from the \see Master.
	 * @return		"DONE" message to the \see Master.
	 */
	bool* svc( bool* task );

protected:
	int id;
	Grid* g;
	size_t start, end;
};

#endif //GAMEOFLIFE_WORKER_H