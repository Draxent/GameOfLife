/**
 *	@file grid_vect.cpp
 *  @brief Implementation of \see Grid class.
 *  @author Federico Conte (draxent)
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

#include "../include/grid_vect.h"

GridVect::GridVect( size_t height, size_t width ) : Grid( height, width ) { }

void GridVect::init( unsigned int seed )
{
	// Initialize random seed
	srand48( ( seed == 0 ) ? time(NULL) : seed );

	// Fill the Grid width random values
	for ( size_t i = 0; i < this->numCells; i += VLEN )
		this->Read[i:VLEN] = ( drand48() > 0.5 );
}