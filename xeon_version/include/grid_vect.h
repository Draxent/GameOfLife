/**
 *	@file grid_vect.h
 *	@brief Header of \see GridVect class.
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

#ifndef GAMEOFLIFE_GRID_VECT_H
#define GAMEOFLIFE_GRID_VECT_H

#include <iostream>
#include <assert.h>
#include <fstream>
#include <climits>
#include <time.h>
#include <new>

#include "grid.h"

/// It is the vectorized version of the \see Grid class.
class GridVect : public Grid
{
public:
	/**
	 * Initializes a new instance of the \see GridVect class.
	 * @param height	number of original grid rows.
	 * @param width		number of original grid columns.
	 */
	GridVect( size_t height, size_t width  );

	/**
	 * Set up this grid using random values.
	 * It calls the drand48 instead of rand in order to vectorize this function.
	 * @param seed		seed used to initialize the grid.
	 */
	void init( unsigned int seed );

	/**
	 * Count the number of neighbours (the 8 adjacent boxes) of a box grid set to <code>true</code>.
	 * @param index, offset			their sum identify the grid box in which compute this function.
	 * @param index_top, offset		their sum identify the top box.
	 * @param index_bottom, offset	their sum identify the bottom box.
	 */
	__declspec( vector( uniform(index, index_top, index_bottom), linear(offset:1) ) )
	inline int countNeighbours( size_t index, size_t index_top, size_t index_bottom, int offset ) const
	{
		size_t pos = index + offset, pos_top = index_top + offset, pos_bottom = index_bottom + offset;
		return  this->Read[ pos_top - 1 ] +
				this->Read[ pos_top ] +
				this->Read[ pos_top + 1 ] +
				this->Read[ pos - 1 ] +
				this->Read[ pos + 1 ] +
				this->Read[ pos_bottom - 1 ] +
				this->Read[ pos_bottom ] +
				this->Read[ pos_bottom + 1 ];
	}
};

#endif //GAMEOFLIFE_GRID_VECT_H