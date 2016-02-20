/**
 *	@file master.h
 *	@brief Header of \see Master class.
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

#ifndef GAMEOFLIFE_MASTER_H
#define GAMEOFLIFE_MASTER_H

#include <chrono>

#include "ff/farm.hpp"
#include "shared_functions.h"

/// The Master coordinates the work of the \see Worker and performs the barrier on them at the end of each GOL iteration.
class Master:public ff::ff_node_t<bool>
{
public:
	/**
	 * Initializes a new instance of \see Master class.
	 * @param lb			load balancer used by Master.
	 * @param nw			number of Workers that Master will manage.
	 * @param g				the \see Grid object.
	 * @param iterations	number of GOL iterations to execute.
	 */
	Master( ff::ff_loadbalancer* const lb, int nw, Grid* g, size_t iterations );

	/**
	 * FastFlow method of the \see ff::ff_node_t.
	 * The pseudo-code of the method is the following:
	 * 		1. Send "GO" to all Workers.
	 * 		2. Wait "DONE" from all Workers.
	 * 		3. Execute the end_generation function on the Grid: swap, copyBorder, print.
	 * 		4. IF ( the number of completed iterations is equal to <em>iterations</em> ). // End of GOL
	 * 			4.1 Send "End-Of-Stream" to all Workers.
	 * 		4. Else
	 * 			4.2 Start from point 1.
	 * @param task	"DONE" message or <code>NULL</code>.
	 * @return	the received task.
	 */
	bool* svc( bool* task );

private:
	void sendGO();
	const int num_workers;
	Grid* g;
	ff::ff_loadbalancer* const lb;
	const size_t iterations;
	long barrier_time, copyborder_time;
	unsigned int completed_workers, completed_iterations;
	std::chrono::high_resolution_clock::time_point t1, t2;
	bool GO = true;
};

#endif //GAMEOFLIFE_MASTER_H
