/**
 *	@file barrier.hpp
 *	@brief Header of \see Barrier class.
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


#ifndef INCLUDE_BARRIER_HPP_
#define INCLUDE_BARRIER_HPP_

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include <grid.hpp>

/// This class implements the barrier synchronization model among workers.
class Barrier
{
public:
	/**
	* Initializes a new instance of the \see Barrier class.
	* When the barrier is reached by all the workers, some "global".
	* operations are performed on the shared \see Grid <em>g</em>: swap, copyBorder and print.
	* @param g		grid, see \see Grid.
	* @param nw		number of workers.
	* @param ff		looking mechanisms are not applied in FastFlow version.
	*/
	Barrier( Grid* g, int nw, bool ff );

	/**
	* Apply the barrier, so every thread that will invoke <em>apply</em> will wait
	* until the last worker will notify all about "all worker finish to compute".
	*/
	void apply();
	
	/**
	* Return whether or not all the workers have reached the barrier during the last call of function apply.
	* @return	<code>true</code> if the barrier is been reached by all the workers, <code>false</code> otherwise.
	*/
	bool is_reached();
	
	/**
	* Return the number of completed steps of GOL.
	* @return	number of completed steps.
	*/
	int get_completed_steps();
	
	/**
	* Retrieve the total time of the barrier phase in microseconds. 
	* @return	total time of the barrier phase.
	*/
	unsigned long get_time();

private:
	void unique_processing();
	
	bool ff, reached;
	unsigned long time;
	int num_workers, completed_workers, completed_steps;
	std::chrono::high_resolution_clock::time_point t1, t2;
	Grid* g;
	std::mutex mux;
	std::condition_variable cv;
};

#endif /* INCLUDE_BARRIER_HPP_ */