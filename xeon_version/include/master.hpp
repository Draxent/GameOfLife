/**
 *	@file master.hpp
 *	@brief Header of \see Master class.
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

#ifndef INCLUDE_MASTER_HPP_
#define INCLUDE_MASTER_HPP_

#include <ff/farm.hpp>
#include <barrier.hpp>

/// The Master coordinates the work of the \see Worker and performs the barrier on them at the end of each GOL iteration.
class Master:public ff::ff_node_t<bool>
{
public:
	/**
	 * Initializes a new instance of \see Master class.
	 * @param lb		load balancer used by Master.
	 * @param nw		number of Workers that Master will manage.
	 * @param barrier	shared object of \see Barrier class
	 * @param steps		number of GOL steps to execute.
	 */
	Master( ff::ff_loadbalancer* const lb, int nw, Barrier* barrier, size_t steps );

	/**
	 * FastFlow method of the \see ff::ff_node_t.
	 * The pseudo-code of the method is the following:
	 * 		1. Send "GO" to all Workers.
	 * 		2. Wait "DONE" from all Workers.
	 * 		3. Apply the Barrier.
	 * 		4. IF ( the numer of completed steps is equal to <em>steps</em> ). // End of GOL
	 * 			4.1 Send "End-Of-Stream" to all Workers.
	 * 		4. Else
	 * 			4.2 Start from point 1.
	 * @param task	"DONE" message or <code>NULL</code>.
	 * @return	the received task.
	 */
	bool* svc( bool* task );

private:
	void sendGO();
	const int nw;
	ff::ff_loadbalancer* lb;
	Barrier* barrier;
	const size_t steps;
	bool GO = true;
};

#endif /* INCLUDE_MASTER_HPP_ */