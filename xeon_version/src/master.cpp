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


#include <master.hpp>

Master::Master( ff::ff_loadbalancer* const lb, int nw, Barrier* barrier, size_t steps ) : lb(lb), nw(nw), barrier(barrier), steps(steps) { }

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
		barrier->apply();
		
		if ( barrier->is_reached() )
		{
			// Complete the work broadcasting End-Of-Stream to all Workers or restart a new iteration.
			if ( barrier->get_completed_steps() == this->steps )
				this->lb->broadcast_task( EOS );
			else
				this->sendGO();			
		}
		return GO_ON;
	}
}

void Master::sendGO()
{
	// Send "GO" to all Workers
	for ( int i = 0; i < this->nw; i++ )
		this->lb->ff_send_out_to( &(this->GO), i );
}