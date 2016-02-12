/**
 *	@file barrier.cpp
 *	@brief Implementation of \see Barrier class.
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

#include "barrier.hpp"

Barrier::Barrier( Grid* g, int nw, bool ff ) : g(g), num_workers(nw), ff(ff)
{
	this->completed_workers = 0;
	this->completed_steps = 0;
	this->time = 0;
}

void Barrier::apply()
{
	std::unique_lock<std::mutex> lock;
	// We execute a lock only when the number of workers is greater than one and we are not using FastFlow version
	if ( this->num_workers > 1 && !this->ff )
		lock = std::unique_lock<std::mutex>( this->mux );
	
	if ( this->completed_workers == 0 )
		// Start - Barrier.
		this->t1 = std::chrono::high_resolution_clock::now();
	
	// Reset the boolean flag.
	this->reached = false;
	
	// Increment the number of completed workers.
	this->completed_workers++;

	if ( this->completed_workers == this->num_workers )
	{
		// Set the boolean flag.
		this->reached = true;

		// It is the last worker that finished the computation.
		// Reset the number of completed workers.
		this->completed_workers = 0;

		// Increment the number of completed steps.
		this->completed_steps++;

		// Swap the reading and writing matrixes.
		this->g->swap();

		// Every step we need to configure the border to properly respect the logic of the 2D toroidal grid
		this->g->copyBorder();

#if DEBUG
		// Print the result of the Game of Life iteration.
		std::string title = "ITERATION " + std::to_string( (long long) this->completed_steps ) + " -";
		this->g->print( title.c_str(), true );
#endif // DEBUG

		// End - Barrier
		this->t2 = std::chrono::high_resolution_clock::now();
		this->time += std::chrono::duration_cast<std::chrono::microseconds>( this->t2 - this->t1 ).count();
		
		// We need to unlock all the waiting threads only when the number of workers is greater than one and we are not using FastFlow version.
		if ( this->num_workers > 1 && !this->ff )
			// Notify all the workers about the end of the iteration.
			this->cv.notify_all();
	}
	// We need put the thread in the waiting state only if we are not using FastFlow version.
	else if ( !this->ff )
	{
		// Wait that all the workers finish this iteration.
		this->cv.wait(lock);
		// cond.wait(lock, [&](){ return counts[my] = nw} );
	}
}

bool Barrier::is_reached()
{
	return this->reached;
}

int Barrier::get_completed_steps()
{
	return this->completed_steps;
}

unsigned long Barrier::get_time()
{
	return this->time;
}