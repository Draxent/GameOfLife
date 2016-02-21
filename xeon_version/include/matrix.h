/**
 *	@file matrix.h
 *	@brief Header of \see Matrix class.
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

#ifndef GAMEOFLIFE_MATRIX_H
#define GAMEOFLIFE_MATRIX_H

#include <iostream>
#include <new>

#include "grid.h"

/**
 * This class is like a boolean matrix.
 * It is an easier data structure with which represent the Game of Life grid.
 * It is the verifier that checks if the application calculated the right generation evolution.
 */
class Matrix
{
public:
	/**
	 * Initializes a new instance of the \see Matrix class.
	 * There are, actually, two matrixes: one used for reading, one used for writing.
	 * Since this object is needed only as simple data structure to compare with the
	 * \see Grid, it use it as a reference object.
	 * The boolean reading array of the Grid object is cloned into this boolean matrix.
	 * @param g		the \see Grid object to clone.
	 */
	Matrix( Grid* g );

	/**
	 * Return <code>true</code> if the boolean matrix is equal to the boolean reading array.
	 * @return		<code>true</code> if the boolean matrix is equal to the boolean reading array.
	 */
	bool equal();

	/**
	 * Return the width of the boolean matrix.
	 * @return	the width of the boolean matrix.
	 */
	size_t width() const;

	/**
	 * Return the height of the boolean matrix.
	 * @return	the height of the boolean matrix.
	 */
	size_t height() const;

	/**
	 * Return the value of the i-th row and j-th column from the reading boolean matrix.
	 * @param i			index of the row to retrieve.
	 * @param j			index of the column to retrieve.
	 * @return 			the retrieved value.
	 */
	bool get( size_t i, size_t j ) const;

	/**
	 * Set the value of the i-th row and j-th column of the writing boolean matrix.
	 * @param i			index of the row to set.
	 * @param j			index of the column to set.
	 * @param value		value that the element will take.
	 */
	void set( size_t i, size_t j, bool value );

	/**
	 * Count the number of neighbours (the 8 adjacent cells) of a cell matrix set to <code>true</code>.
	 * @param i, j		indexes that identify the cell matrix in which compute this function.
	 */
	int countNeighbours( size_t i, size_t j ) const;

	/// Print the boolean matrix on the standard output.
	void print();

	/// Swap the reading and writing matrixes
	void swap();

	/**
	 * The verifier function. Execute a simplest version of Game of Life.
	 * Its goal is to compare this result with the one of our application, so using
	 * this function as a verifier of our result.
	 * @param iterations	number of generation of GOL to compute.
	 */
	void GOL( unsigned int iterations );

	/// Destructor of the \see Matrix class.
	~Matrix();

private:
	/// Allocate space in the heap for the reading and writing boolean matrix.
	void allocate();

	size_t rows, cols;
	bool **read, **write;
	Grid* g;
};

#endif //GAMEOFLIFE_MATRIX_H
