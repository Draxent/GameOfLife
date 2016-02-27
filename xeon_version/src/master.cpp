/**
 *	@file master.cpp
 *  @brief Implementation of \see Master class.
 *  @author Federico Conte (draxent)
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


#include "../include/master.h"

Master::Master( ff::ff_loadbalancer* const lb, unsigned int nw, Grid* g, unsigned int iterations, size_t start, size_t* chunks, unsigned int num_tasks )
			: lb(lb), num_workers(nw), g(g), iterations(iterations), start(start),
			  chunks(chunks), num_tasks(num_tasks)
{
	this->completed_iterations = 0;
	this->start_chunk = 0;
	this->end_chunk = start;
	this->counter_complete_tasks = 0;
	this->counter_sent_tasks = 0;
	this->copyborder_time = 0;
	this->barrier_time = 0;
	this->first_worker = true;
}

Task_t* Master::svc( Task_t* task )
{
	if ( task == nullptr )
	{
		send_tasks();
		// Keep it alive.
		return GO_ON;
	}
	else
	{
		// Increment the counter of complete tasks and delete the task.
		this->counter_complete_tasks++;
		delete( task );

		// Get the worker identifier of who is responding.
		int worker_id = lb->get_channel_id();

		// If it did not complete scattering the Grid.
		if ( this->counter_sent_tasks < this->num_tasks )
		{
			Task_t* task = create_new_task();
			this->lb->ff_send_out_to( task, worker_id );
		}
#if TAKE_ALL_TIME
		else if ( this->first_worker )
		{
			// When it has completed to scatter the Grid, it start counting the Barrier Phase
			// from the first Worker that answer back.
			// Start - Barrier Phase
			t1 = std::chrono::high_resolution_clock::now();
			this->first_worker = false;
		}
#endif // TAKE_ALL_TIME

		// If the counter is equal to the total number of tasks, we complete the iteration.
		if ( this->counter_complete_tasks == this->num_tasks )
		{
#if TAKE_ALL_TIME
			// End - Barrier Phase
			t2 = std::chrono::high_resolution_clock::now();
			barrier_time += std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
#endif // TAKE_ALL_TIME

			// Reset the accumulators that tracks the starting and ending point of the current chunk.
			this->start_chunk = 0;
			this->end_chunk = start;

			// Reset the first worker variable
			this->first_worker = true;

			// Reset counters.
			this->counter_sent_tasks = 0;
			this->counter_complete_tasks = 0;

			// Increment the number of completed iterations.
			this->completed_iterations++;

			// Compute the action necessary to complete the computation of this generation.
			copyborder_time += end_generation( g, this->completed_iterations );

			// Send EOS if we completed all the iterations.
			if ( this->completed_iterations == this->iterations )
			{
#if TAKE_ALL_TIME
				// Print the total time in order to compute the end_generation functions.
				printTime( copyborder_time, "copy border" );

				// Print the total time in order to compute the barrier phase.
				printTime( barrier_time, "barrier phase" );
#endif // TAKE_ALL_TIME

				return EOS;
			}
			else // We go on with the computation of the next generation.
			{
				send_tasks();
				// Keep it alive.
				return GO_ON;
			}
		}
		else // Keep it alive.
			return GO_ON;
	}
}

Task_t* Master::create_new_task()
{
	this->start_chunk = this->end_chunk;
	this->end_chunk = this->start_chunk + this->chunks[this->counter_sent_tasks];
	this->counter_sent_tasks++;
	return new Task_t( this->start_chunk, this->end_chunk );
}

void Master::send_one_task_x_worker()
{
	// It sends a task of the current iteration to each worker.
	for ( int i = 0; i < this->num_workers; i++ )
	{
		Task_t* task = create_new_task();
		// Send Task to the next Worker.
		this->lb->ff_send_out_to( task, i );
	}
}

void Master::send_tasks()
{
	send_one_task_x_worker();
	// If we have two task per Worker, do overbooking technique.
	if ( this->num_tasks > 2*this->num_workers )
		send_one_task_x_worker();
}