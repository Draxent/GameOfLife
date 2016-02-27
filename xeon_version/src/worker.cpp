/**
 *	@file worker.cpp
 *	@brief Implementation of \see Worker class.
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


#include "../include/worker.h"

Worker::Worker( int id, Grid* g, bool vectorization )
		: id(id), g(g), vectorization(vectorization)
{
	if ( this->vectorization )
		this->numNeighbours = new int[VLEN];
}

Task_t* Worker::svc( Task_t* task )
{
#if VECTORIZATION
	// The working size has to be significant in order to vectorized the thread_body function.
	if ( this->vectorization )
		compute_generation_vect( this->g, this->numNeighbours, task->start, task->end );
	else
		compute_generation( this->g, task->start, task->end );
#else
	compute_generation( this->g, task->start, task->end );
#endif // VECTORIZATION
	return task;
}

Worker::~Worker()
{
	if ( this->vectorization )
		delete[] this->numNeighbours;
}