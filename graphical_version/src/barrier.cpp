/**
 *	@file barrier.cpp
 *  @brief Implementation of \see Barrier class.
 *  @author Federico Conte (draxent)
 *  
 *	Copyright 2015 Federico Conte
 *	https://github.com/Draxent/ConnectedComponents
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

Barrier::Barrier( Matrix* m, int nw )
{
	this->m = m;
	this->num_workers = nw;
	this->completed_workers = 0;
}

void Barrier::apply( int iteration_number )
{
	std::unique_lock<std::mutex> lock(this->mux);
	this->completed_workers++;

	if ( this->completed_workers == this->num_workers )
	{
		// It is the last worker that finished the computation.
		// Reset the number of completed workers.
		this->completed_workers = 0;
		// Swap the reading and writing matrixes.
		this->m->swap();
		// Print the result of the Game of Life iteration.
		this->m->print();
		// Print the iteration number that was completed by the worker.
		std::cout << "Iteration " << iteration_number << " completed !!!" << std::endl;
		// Notify all the workers about the end of the iteration.
		this->cv.notify_all();
	}
	else
		// Wait that all the workers finish this iteration.
    	this->cv.wait(lock);
}
