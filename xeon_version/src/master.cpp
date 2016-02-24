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

Master::Master( ff::ff_loadbalancer* const lb, unsigned int nw, Grid* g, unsigned int iterations, size_t start, size_t end, size_t chunk_size, unsigned long num_tasks )
			: lb(lb), num_workers(nw), g(g), iterations(iterations), start(start), end(end),
			  chunk_size(chunk_size), num_tasks(num_tasks)
{
	this->completed_iterations = 0;
	this->start_chunk = 0;
	this->end_chunk = start;
	this->counter = 0;
	this->copyborder_time = 0;
	this->barrier_time = 0;
	this->first_worker = true;
}

Task_t* Master::svc( Task_t* task )
{
	if ( task == nullptr )
	{
		// Send one task of the first iteration to each worker; we know that #Tasks >= #Workers.
		send_one_task_x_worker();
		// Keep it alive.
		return GO_ON;
	}
	else
	{
		// Increment Task counter and delete Task.
		this->counter++;
		delete( task );

		// Get the worker identifier of who is responding.
		int worker_id = lb->get_channel_id();

		// If it did not complete scattering the Grid.
		if ( this->end_chunk < this->end )
		{
			Task_t* task = create_new_task();
			this->lb->ff_send_out_to( task, worker_id );
		}
		else if ( this->first_worker )
		{
			// When it has completed to scatter the Grid, it start counting the Barrier Phase
			// from the first Worker that answer back.
			// Start - Barrier Phase
			t1 = std::chrono::high_resolution_clock::now();
			this->first_worker = false;
		}

		// If the Task counter is equal to the number of task sent, we complete the iteration.
		if ( this->counter == this->num_tasks )
		{
			// End - Barrier Phase
			t2 = std::chrono::high_resolution_clock::now();
			barrier_time += std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

			// Reset the accumulators that tracks the starting and ending point of the current chunk.
			this->start_chunk = 0;
			this->end_chunk = start;

			// Reset the first worker variable
			this->first_worker = true;

			// Reset counter.
			this->counter = 0;

			// Increment the number of completed iterations.
			this->completed_iterations++;

			// Compute the action necessary to complete the computation of this generation.
			copyborder_time += end_generation( g, this->completed_iterations );

			// Send EOS if we completed all the iterations.
			if ( this->completed_iterations == this->iterations )
			{
				// Print the total time in order to compute the end_generation functions.
				printTime( copyborder_time, "copy border" );

				// Print the total time in order to compute the barrier phase.
				printTime( barrier_time, "barrier phase" );

				return EOS;
			}
			else // We go on with the computation of the next generation.
			{
				// Send one task of the next iteration to each worker
				this->send_one_task_x_worker();
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
	this->end_chunk = std::min( this->start_chunk + this->chunk_size, this->end );
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