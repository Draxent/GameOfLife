/**
 *	@file spinning_barrier.cpp
 *	@brief Implementation of \see SpinningBarrier class.
 *	@author Federico Conte (draxent)
 *
 *  The implementation is inspired by Momchil Velikov
 *  Its version can be found here:
 *  http://stackoverflow.com/questions/8115267/writing-a-spinning-thread-barrier-using-c11-atomics
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

#include "../include/spinning_barrier.h"

SpinningBarrier::SpinningBarrier( unsigned int nw )
		: nw(nw), num_waiting(0), iterations(0), time(0) { }

bool SpinningBarrier::is_last_thread()
{
	// Load the current iteration
	unsigned int current_iteration = this->iterations.load ();
	unsigned int current_waiting = this->num_waiting.fetch_add( 1 );

	if ( current_waiting == 0 )
		// Start - Barrier Phase.
		this->t1 = std::chrono::high_resolution_clock::now();

	// If the value before the increment is equal to number of threads minus one, it is the last thread.
	if ( current_waiting == this->nw - 1 )
	{
		// End - Barrier Phase
		this->t2 = std::chrono::high_resolution_clock::now();
		this->time += std::chrono::duration_cast<std::chrono::microseconds>( this->t2 - this->t1 ).count();

		return true;
	}
	else
	{
		// Enters in a busy-looping until this barrier has not been reached by all threads.
		while ( this->iterations.load () == current_iteration );
		return false;
	}
}

void SpinningBarrier::notify_all()
{
	// Reset the number of waiting threads.
	this->num_waiting.store( 0 );
	// Increment the number of completed iterations.
	this->iterations.fetch_add( 1 );
}

long SpinningBarrier::get_time()
{
	return this->time;
}