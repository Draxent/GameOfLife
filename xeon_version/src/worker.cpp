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

Worker::Worker( int id, Grid* g, size_t start, size_t end, bool vectorization )
		: id(id), g(g), start(start), end(end), vectorization(vectorization)
{
	if ( vectorization )
		this->numsNeighbors = (int*) _mm_malloc( VLEN * sizeof(int), 64 );
}

bool* Worker::svc( bool* task )
{
	compute_generation( this->g, this->numsNeighbors, this->start, this->end, this->vectorization );
	return task;
}

Worker::~Worker()
{
	if ( this->vectorization )
		_mm_free( this->numsNeighbors );
}