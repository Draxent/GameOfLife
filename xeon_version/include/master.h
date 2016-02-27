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
#include "task.h"
#include "shared_functions.h"

/// The Master coordinates the work of the \see Worker and performs the barrier on them at the end of each GOL iteration.
class Master:public ff::ff_node_t<Task_t>
{
public:
	/**
	 * Initializes a new instance of \see Master class.
	 * @param lb			load balancer used by Master.
	 * @param nw			number of Workers to coordinate.
	 * @param g				the \see Grid object.
	 * @param iterations	number of GOL iterations to execute.
	 * @param start			start indexing to the Grid working area.
	 * @param chunks		array of chunks size to assign to Workers.
	 * @param num_tasks		number of task per Worker that it will generate for each generation.
	 */
	Master( ff::ff_loadbalancer* const lb, unsigned int nw, Grid* g, unsigned int iterations,
			size_t start, size_t* chunks, unsigned int num_tasks );

	/**
	 * FastFlow method of the \see ff::ff_node_t.
	 * The pseudo-code of the method is the following:
	 * 		1. Distribute one task to each Worker.
	 * 		2. When a Worker reply "DONE" the Master assigns it another task (On Demand).
	 * 		   The task size is decreasing over time.
	 * 		3. When all tasks are computed it executes the end_generation function on the Grid: swap, copyBorder, print.
	 * 		4. IF ( the number of completed iterations is equal to <em>iterations</em> ). // End of GOL
	 * 			4.1 Send "End-Of-Stream" to all Workers.
	 * 		4. Else
	 * 			4.2 Start from point 1.
	 * @param task	the task that has been computed or <code>NULL</code>.
	 * @return	the task to compute.
	 */
	Task_t* svc( Task_t* task );

private:
	// Create a new task.
	Task_t* create_new_task();

	// Send one task of the current iteration to each worker; we know that #Tasks >= #Workers.
	void send_one_task_x_worker();

	// Overbooking: send two tasks to each worker.
	void send_tasks();

	Grid* g;
	size_t* chunks;
	ff::ff_loadbalancer* const lb;
	const unsigned int iterations, num_workers, num_tasks;
	const size_t start;
	size_t start_chunk, end_chunk;
	unsigned int completed_iterations, counter_complete_tasks, counter_sent_tasks;
	long copyborder_time, barrier_time;
	bool first_worker;
	std::chrono::high_resolution_clock::time_point t1, t2;
};

#endif //GAMEOFLIFE_MASTER_H
