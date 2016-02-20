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

Master::Master( ff::ff_loadbalancer* const lb, int nw, Grid *g, size_t iterations )
		: lb(lb), num_workers(nw), g(g), iterations(iterations), barrier_time(0),
		  copyborder_time(0), completed_workers(0), completed_iterations(0) { }

bool* Master::svc( bool* task )
{
	if ( task == nullptr )
	{
		// Initially the Master send "GO" to all the Workers.
		this->sendGO();
		// Keep it alive.
		return GO_ON;
	}
	else
	{
		if ( this->completed_workers == 0 )
			// Start - Barrier Phase.
			this->t1 = std::chrono::high_resolution_clock::now();

		// Increment the number of completed workers.
		this->completed_workers++;

		if ( this->completed_workers == this->num_workers )
		{
			// End - Barrier Phase
			this->t2 = std::chrono::high_resolution_clock::now();
			this->barrier_time += std::chrono::duration_cast<std::chrono::microseconds>( this->t2 - this->t1 ).count();

			// It is the last worker that finished the computation.
			// Reset the number of completed workers.
			this->completed_workers = 0;

			// Increment the number of completed iterations.
			this->completed_iterations++;

			copyborder_time += end_generation( g, this->completed_iterations );

			// Complete the work broadcasting End-Of-Stream to all Workers or restart a new iteration.
			if ( this->completed_iterations == this->iterations )
			{
				this->lb->broadcast_task( EOS );

				// Print the total time in order to compute the end_generation functions.
				printTime( copyborder_time, "copy border" );

				// Print the total time in order to compute the barrier phase.
				printTime( barrier_time, "barrier phase" );
			}
			else
				this->sendGO();
		}

		return GO_ON;
	}
}

void Master::sendGO()
{
	// Send "GO" to all Workers
	for ( int i = 0; i < this->num_workers; i++ )
		this->lb->ff_send_out_to( &(this->GO), i );
}