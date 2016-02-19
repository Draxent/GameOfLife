/**
 *	@file worker_vect.cpp
 *	@brief Implementation of \see WorkerVect class.
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


#include "../include/worker_vect.h"

WorkerVect::WorkerVect( int id, GridVect* g, size_t start, size_t end ) : Worker( id, g, start, end )
{
	this->numsNeighbors = new int[VLEN];
}

bool* WorkerVect::svc( bool* task )
{
	compute_generation_vect( (GridVect*) this->g, this->numsNeighbors, this->start, this->end );
	return task;
}

WorkerVect::~WorkerVect()
{
	delete[] this->numsNeighbors;
}