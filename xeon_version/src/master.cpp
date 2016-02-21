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

Master::Master( ff::ff_loadbalancer* const lb, Grid* g, unsigned int iterations, size_t start, size_t end, size_t chunk_size, unsigned long num_tasks )
			: lb(lb), g(g), iterations(iterations), start(start), end(end),
			  chunk_size(chunk_size), num_tasks(num_tasks)
{
	this->completed_iterations = 0;
	this->start_chunk = 0;
	this->end_chunk = start;
	this->counter = 0;
	this->copyborder_time = 0;
	this->barrier_time = 0;
	this->first_worker = -1;
}

Task_t* Master::svc( Task_t* task )
{
	if ( task == nullptr )
	{
		// First iteration
		send_work();
		// Keep it alive.
		return GO_ON;
	}
	else
	{
		// We start calculating the barrier time when the first worker finish all its jobs.
		// This measure is not precise, but it is the best we though about.
		if ( this->counter == 0 || lb->get_channel_id() == this->first_worker )
		{
			this->first_worker = lb->get_channel_id();
			// Start - Barrier Phase
			t1 = std::chrono::high_resolution_clock::now();
		}

		// Increment Task counter and delete Task.
		this->counter++;
		delete( task );

		// If the Task counter is equal to the number of task sent, we complete the iteration.
		if ( this->counter == this->num_tasks )
		{
			// End - Barrier Phase
			t2 = std::chrono::high_resolution_clock::now();
			barrier_time += std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

			// Reset counter.
			this->counter = 0;

			// Increment the number of completed iterations.
			this->completed_iterations++;

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
				// Next iteration
				this->send_work();
				// Keep it alive.
				return GO_ON;
			}
		}
		else // Keep it alive.
			return GO_ON;
	}
}

void Master::send_work()
{
	this->start_chunk = 0;
	this->end_chunk = start;
	// Until it does not complete scattering the Grid.
	while ( this->end_chunk < this->end )
	{
		// Create new Task.
		this->start_chunk = this->end_chunk;
		this->end_chunk = std::min( this->start_chunk + this->chunk_size, this->end );
		Task_t *task = new Task_t( this->start_chunk, this->end_chunk );

		// Send Task to the next Worker.
		ff_send_out( task );
	}
}