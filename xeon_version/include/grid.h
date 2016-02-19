/**
 *	@file grid.h
 *	@brief Header of \see Grid class.
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

#ifndef GAMEOFLIFE_GRID_H
#define GAMEOFLIFE_GRID_H

#include <iostream>
#include <assert.h>
#include <time.h>
#include <new>

// The vector processing unit (VPU) in Xeon Phi™ coprocessor provides data parallelism at a very fine grain,
// working on 512 bits of 16 single-precision floats or 32-bit integers at a time.
#if MIC
static const int VLEN = 32;
#else
static const int VLEN = 16;
#endif

#define RAND_MAX_HALF RAND_MAX/2

/// This class represent the grid of GOL (2D toroidal grid) and use internally two boolean array: one for reading one for writing.
class Grid
{
public:
	/// Boolean array representing the grid used for reading the grid
	bool* Read;
	/// Boolean array representing the grid used for writing onto the grid
	bool* Write;

	/**
	 * Initializes a new instance of the \see Grid class.
	 * The size of the Grid is enlarged in order to have an additional border.
	 * Internally, it use two arrays: one for reading, one for writing.
	 * @param height			number of original grid rows.
	 * @param width				number of original grid columns.
	 */
	Grid( size_t height, size_t width );

	/**
	 * Set up this grid using random values.
	 * @param seed				seed used to initialize the grid.
	 */
	void init( unsigned int seed );

	/**
	 * Return the actual grid width.
	 * @return	the actual grid width.
	 */
	size_t width() const;

	/**
	 * Return the actual grid height.
	 * @return	the actual grid height.
	 */
	size_t height() const;

	/**
	 * Return the number of cells of the grid.
	 * @return	the number of cells.
	 */
	size_t size() const;

	/**
	 * Let assume that we have a Grid of 3x3.
	 * We enlarge the matrix with a border in order to favor locality in the countNeighbors computation.
	 * So we end up having a Grid of 5x5 as in the example below:
	 *  _ _ _ _ _
	 * |_|_|_|_|_|
	 * |_|x|x|x|_|
	 * |_|x|x|x|_|
	 * |_|x|x|x|_|
	 * |_|_|_|_|_|
	 *
	 * This function fills the border with the appropriate values following the logic of the 2D toroidal grid.
	 */
	void copyBorder();

	/**
	 * Count the number of neighbours (the 8 adjacent boxes) of a box grid set to <code>true</code>.
	 * @param pos			identify the grid box in which compute this function.
	 * @param pos_top		identify the top box.
	 * @param pos_bottom	identify the bottom box.
	 */
	inline int countNeighbours( size_t pos, size_t pos_top, size_t pos_bottom ) const
	{
		return  this->Read[ pos_top - 1 ] +
				this->Read[ pos_top ] +
				this->Read[ pos_top + 1 ] +
				this->Read[ pos - 1 ] +
				this->Read[ pos + 1 ] +
				this->Read[ pos_bottom - 1 ] +
				this->Read[ pos_bottom ] +
				this->Read[ pos_bottom + 1 ];
	}

	/**
	 * Print the boolean matrix on the standard output.
	 * @param msg		additional message to print as title.
	 * @param border	if <code>true</code> print the additional border of the grid.
	 */
	void print( const char* msg, bool border = false );

	/// Swap the reading and writing matrixes
	void swap();

	/// Destructor of the \see Grid class.
	~Grid();

protected:
	// Allocate space in the heap for the reading and writing boolean arrays.
	void allocate();

	size_t rows, cols, numCells;
};

#endif //GAMEOFLIFE_GRID_H