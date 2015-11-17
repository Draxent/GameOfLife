/*
 *	@file barrier.hpp
 *  @brief Header of \ref Barrier class.
 *  @author Federico Conte (draxent)
 */

#ifndef INCLUDE_BARRIER_HPP_
#define INCLUDE_BARRIER_HPP_

#include <iostream>
#include <mutex>
#include <condition_variable>
#include "matrix.hpp"

/// This class implements the barrier synchronization model among workers.
class Barrier
{
public:
	/**
	* Initializes a new instance of the \ref Barrier class.
	* When the barrier is reached by all the workers, some "global".
	* operations are performed on the shared boolean matrix <em>m</em>: print and swap.
	* @param m		boolean matrix, see \ref Matrix.
	* @param nw		number of workers.
	*/
	Barrier( Matrix* m, int nw );

	/**
	* Apply the barrier, so every thread that will invoke <em>apply</em> will wait
	* until the last worker will notify all about "all worker finish to compute".
	* @param iteration_number	print the iteration number that was completed by the worker.
	*/
	void apply( int iteration_number );

private:
	int num_workers, completed_workers;
	Matrix* m;
	std::mutex mux;
	std::condition_variable cv;
};

#endif /* INCLUDE_BARRIER_HPP_ */
