/*
 * @file matrix.hpp
 * @brief Header of \ref Matrix class.
 * @author Federico Conte (draxent)
 */

#ifndef INCLUDE_MATRIX_HPP_
#define INCLUDE_MATRIX_HPP_

#include <iostream>
#include <assert.h>
#include <fstream>
#include <climits>
#include <time.h>

/// This class is like a boolean matrix.
class Matrix
{
public:
	/**
	 * Initializes a new instance of the \ref Matrix class.
	 * Set up this matrix using random values.
	 * There are, actually, two matrixes: one used for reading, one used for writing.
	 * @param height	number of rows of the boolean matrix.
	 * @param width		number of columns of the boolean matrix.
	 */
	Matrix( int height, int width );

	/**
	 * Initializes a new instance of the \ref Matrix class.
	 * Set up this matrix using the values written in a text file.
	 * There are, actually, two matrixes: one used for reading, one used for writing.
	 * @param input_path	the path of the input file.
	 */
	Matrix( const char *input_path );

	/**
	 * Return the width of the boolean matrix.
	 * @return	the width of the boolean matrix.
	 */
	int width() const;

	/**
	 * Return the height of the boolean matrix.
	 * @return	the height of the boolean matrix.
	 */
	int height() const;

	/**
	 * Return the value of the i-th row and j-th column from the reading boolean matrix.
	 * @param i			index of the row to retrieve.
	 * @param j			index of the column to retrieve.
	 * @return 			the retrieved value.
	 */
	bool get( int i, int j ) const;

	/**
	 * Set the value of the i-th row and j-th column of the writing boolean matrix.
	 * @param i			index of the row to set.
	 * @param j			index of the column to set.
	 * @param value		value that the element will take.
	 */
	virtual void set( int i, int j, bool value );

	/**
	 * Count the number of neighbors (the 8 adjacent cells) of a cell matrix set to <code>true</code>.
	 * @param i, j		indexes that identify the cell matrix in which compute this function.
	 */
	int countNeighbors( int i, int j ) const;

	/// Print the boolean matrix on the standard output.
	virtual void print();

	/// Swap the reading and writing matrixes
	void swap();

	/// Destructor of the \ref Matrix class.
	virtual ~Matrix();

protected:
	int rows, cols;

private:
	bool **read, **write;

	/**
	 * Allocate space in the heap for the reading and writing boolean matrix.
	 * @param height	number of rows of the boolean matrix.
	 * @param width		number of columns of the boolean matrix.
	 */
	void allocate( int height, int width );

	friend std::ostream& operator<<(std::ostream&, const Matrix&);
};

#endif /* INCLUDE_MATRIX_HPP_ */