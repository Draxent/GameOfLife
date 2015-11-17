/**
 *	@file barrier.cpp
 *  @brief Implementation of \ref Barrier class.
 *  @author Federico Conte (draxent)
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
