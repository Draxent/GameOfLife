/**
 *	@file barrier.hpp
 *  @brief Header of \see Barrier class.
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
	* Initializes a new instance of the \see Barrier class.
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
