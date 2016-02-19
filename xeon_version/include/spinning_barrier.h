/**
 *	@file spinning_barrier.h
 *	@brief Header of \see SpinningBarrier class.
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

#ifndef GAMEOFLIFE_SPINNING_BARRIER_H
#define GAMEOFLIFE_SPINNING_BARRIER_H

#include <atomic>
#include <chrono>

/// This class implements the barrier synchronization model among threads.
class SpinningBarrier
{
public:
	/**
	 * Initializes a new instance of the \see SpinningBarrier class.
	 * @param nw	number of threads to synchronize.
	 */
	SpinningBarrier( unsigned int nw );

	/**
	 * All threads enter in a spinning loop except the last one.
	 * @return		<code>true</code> if the last thread enter and completed the barrier.
	 */
	bool is_last_thread();

	/// Notify the ending of the Barrier Phase to all the threads.
	void notify_all();

	/**
	 * Retrieve the total time of the barrier phase in microseconds ( sum of times of all barrier executions ).
	 * @return	total time of the barrier phase.
	 */
	long get_time();

private:
	// Number of threads to synchronize.
	const unsigned int nw;
	// Number of threads currently spinning.
	std::atomic<unsigned int> num_waiting;
	// Number of barrier synchronizations completed so far.
	std::atomic<unsigned int> iterations;
	// Necessary to In order to compute the time needed to complete the barrier phase.
	std::chrono::high_resolution_clock::time_point t1, t2;
	// Keep trace of the total time to complete the barrier phase ( sum of times of all barrier executions ).
	long time;
};

#endif //GAMEOFLIFE_SPINNING_BARRIER_H
