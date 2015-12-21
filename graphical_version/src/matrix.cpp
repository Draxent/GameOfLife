/**
 *	@file matrix.cpp
 *  @brief Implementation of \see Matrix class.
 *  @author Federico Conte (draxent)
 *  
 *	Copyright 2015 Federico Conte
 *	https://github.com/Draxent/ConnectedComponents
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

#include "matrix.hpp"

Matrix::Matrix( int height, int width )
{
	// Initialize private variables
	this->cols = 0;
	this->rows = 0;
	this->read = NULL;
	this->write = NULL;

	// Allocate the two matrixes
	this->allocate( height, width );

	// Initialize random seed
	srand ( time(NULL) );

	// Fill the matrix width random values
	for (int i = 0; i < this->rows; i++)
		for (int j = 0; j < this->cols; j++)
			this->read[i][j] = (rand() % 100 + 1 > 50);
}

Matrix::Matrix( const char *input_path )
{
	std::ifstream f;

	// Initialize private variables
	this->cols = 0;
	this->rows = 0;
	this->read = NULL;
	this->write = NULL;

	// Starting matrix dimension
	int width = 0, height = 1;

	// Open the input file
	f.open(input_path, std::ios::in);
	if ( f.fail() )
	{
		std::cerr << "Error: it is not possible to open the file." << std::endl;
		exit( 1 );
	}
	if ( f.peek() == EOF )
	{
		std::cerr << "Error: the file is empty." << std::endl;
		exit( 1 );
	}

	// Ignore until newline character
	f.ignore( INT_MAX, '\n' );
	// Count columns in first row
	width = f.gcount() - 1;

	// Consume ENTER if present
    if ( f.peek() == 10 )
		f.get();

	while( !f.eof() && ( f.peek() != EOF) )
	{
		int cur_width;
		// Start a new line
		height++;
		// Ignore until newline character
		f.ignore( INT_MAX, '\n' );
		// Count columns in current row
		cur_width = f.gcount() - ( f.eof() ? 0 : 1 );

		if ( cur_width != width )
		{
			std::cerr << "Error: number of columns at row n." << height << " (" << cur_width << " columns) ";
			std::cerr << "is not equal to that of first row." << std::endl;
			exit(1);
		}

		// Consume ENTER if present
		if ( f.peek() == 10 )
			f.get();
	}

	// Allocate space for the matrix
	this->allocate( height, width );

	// Restart reading file from beginning
	f.seekg( 0, f.beg );

	// Fill the matrix
	for ( int i = 0; i < this->rows; i++ )
	{
		for ( int j = 0; j < this->cols; j++ )
		{
            char tmp;
            f.read( &tmp, sizeof(char) );
            switch ( tmp )
            {
				case '0': this->read[i][j] = false; break;
				case '1': this->read[i][j] = true; break;
				case '\n': j--; break;
				default:
					std::cerr << "Error parsing file at line "<< i << " and column "<< j << ". ";
					std::cerr << "Read " << tmp << " instead of 0, 1 or \\n. ";
					std::cerr << "The matrix has to be composed of 0 and 1 only." << std::endl;
					std::cerr << "Example : "  << std::endl;
					std::cerr << "\t100\\n" << std::endl;
					std::cerr << "\t010\\n" << std::endl;
					std::cerr << "\t001\\n" << std::endl << std::endl;
					exit( 1 );
					break;
            }
		}
	}

	f.close();
}

int Matrix::width() const
{
	return this->cols;
}

int Matrix::height() const
{
	return this->rows;
}

bool Matrix::get( int i, int j ) const
{
	assert ( (i >= 0) && (j >= 0) && (i < this->rows) && (j < this->cols) );

	return this->read[i][j];
}

void Matrix::set( int i, int j, bool value )
{
	assert ( (i >= 0) && (j >= 0) && (i < this->rows) && (j < this->cols) );

	this->write[i][j] = value;
}

int Matrix::countNeighbors( int i, int j ) const
{
	assert ( (i >= 0) && (j >= 0) && (i < this->rows) && (j < this->cols) );

    int c = 0;

	c += this->read[(i + this->rows - 1) % this->rows][(j + this->cols - 1) % this->cols];
	c += this->read[(i + this->rows - 1) % this->rows][j];
	c += this->read[(i + this->rows - 1) % this->rows][(j + 1) % this->cols];

	c += this->read[i][(j + this->cols - 1) % this->cols];
	c += this->read[i][(j + 1) % this->cols];

	c += this->read[(i + 1) % this->rows][(j + this->cols - 1) % this->cols];
	c += this->read[(i + 1) % this->rows][j];
	c += this->read[(i + 1) % this->rows][(j + 1) % this->cols];

    return c;
}

void Matrix::swap()
{
	bool** tmp = this->read;
	this->read  = this->write;
	this->write = tmp;
}

void Matrix::print()
{
	std::cout << "MATRIX (rows: " << this->rows << ", columns: " << this->cols << ") :" << std::endl;
	for ( int i = 0; i < this->rows; i++ )
	{
		for ( int j = 0; j < this->cols; j++ )
		{
			if (this->read[i][j]) std::cout << "1";
			else std::cout << "0";
		}
		std::cout << std::endl;
	}
}

void Matrix::allocate( int height, int width )
{
	assert ( (height > 0) && (width > 0) );

	this->read = new bool*[height];
	this->write = new bool*[height];

	for ( int i = 0; i < height; i++ )
	{
		this->read[i] = new bool[width];
		this->write[i] = new bool[width];
	}

	this->rows = height;
	this->cols = width;
}

Matrix::~Matrix()
{
	for ( int i = 0; i < this->rows; i++ )
	{
		delete[] this->read[i];
		delete[] this->write[i];
	}
	delete[] this->read;
	delete[] this->write;
}

std::ostream& operator<<(std::ostream &strm, const Matrix &m)
{
	strm << "MATRIX (rows: " << m.rows << ", columns: " << m.cols << ") :\n";
	for ( int i = 0; i < m.rows; i++ )
	{
		for ( int j = 0; j < m.cols; j++ )
		{
			if (m.read[i][j]) strm << "1";
			else strm << "0";
		}
		strm << "\n";
	}
	return strm;
}
