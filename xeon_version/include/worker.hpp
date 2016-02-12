/**
 *	@file worker.hpp
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

#ifndef INCLUDE_WORKER_HPP_
#define INCLUDE_WORKER_HPP_

#include <ff/farm.hpp>
#include <grid.hpp>
#include <functions.hpp>

/// This Worker computes GOL generations until \see Master command.
class Worker:public ff::ff_node_t<bool>
{
public:
	/**
	 * Initializes a new instance of the \see Worker class.
	 * If vectorization is <code>true</code> we create an array to store the results of the number of Neighbour counting.
	 * @param g				shared object of the \see Grid class
	 * @param start			row index of the starting working area.
	 * @param end			row index of the ending working area.
	 * @param vectorization	<code>true</code> if the code has to be vectoriazed.
	 */
	Worker( Grid* g, size_t start, size_t end, bool vectorization );

	/**
	 * FastFlow method of the \see ff::ff_node_t.
	 * Call \see compute_generation function, i.e. it responds to the Master request computing a new generation on its portion of the \see Grid.
	 * @param task	"GO" message received from the \see Master.
	 * @return		"DONE" message to the \see Master.
	 */
	bool* svc( bool* task );
	
	/**
	 * Destructor of the \see Worker class.
	 * If vectorization is <code>true</code>, we delete the additional array created during initialization.
	 */
	~Worker();

private:
	Grid* g;
	const bool vectorization;
	const size_t start;
	const size_t end;
	int* numsNeighbors;
	long time;
};

#endif /* INCLUDE_WORKER_HPP_ */