/**
 *	@file matrix.cpp
 *  @brief Implementation of \see Matrix class.
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

#include "../include/matrix.h"

Matrix::Matrix( Grid* g )
{
	// Initialize private variables.
	this->rows = g->height() - 2;
	this->cols = g->width() -2 ;
	this->read = NULL;
	this->write = NULL;
	this->g = g;

	// Allocate the two matrixes.
	this->allocate();

	// Init the boolean reading matrix using the boolean reading array of the Grid g.
	for ( size_t i = 0; i < this->rows; i++ )
	{
		size_t first = (i + 1) * g->width() + 1, last = (i + 2) * g->width() - 1;
		std::copy( g->Read + first, g->Read + last, this->read[i] );
	}
}

bool Matrix::equal()
{
	for ( size_t i = 0; i < this->rows; i++ )
	{
		size_t first = (i + 1) * this->g->width() + 1, last = (i + 2) * this->g->width() - 1;
		if ( !std::equal( this->g->Read + first, this->g->Read + last, this->read[i] ) )
			return false;
	}
	return true;
}

size_t Matrix::width() const
{
	return this->cols;
}

size_t Matrix::height() const
{
	return this->rows;
}

bool Matrix::get( size_t i, size_t j ) const
{
	return this->read[i][j];
}

void Matrix::set( size_t i, size_t j, bool value )
{
	this->write[i][j] = value;
}

int Matrix::countNeighbours( size_t i, size_t j ) const
{
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
	for ( size_t i = 0; i < this->rows; i++ )
	{
		std::cout << this->read[i][0] ? "1" : "0";
		for ( size_t j = 1; j < this->cols; j++ )
			std::cout << " " << ( this->read[i][j] ? "1" : "0" );
		std::cout << std::endl;
	}
}

void Matrix::allocate()
{
	this->read = new bool*[this->rows];
	this->write = new bool*[this->rows];

	for ( size_t i = 0; i < this->rows; i++ )
	{
		this->read[i] = new bool[this->cols];
		this->write[i] = new bool[this->cols];
	}
}

void Matrix::GOL( unsigned int iterations )
{
	for ( unsigned int k = 1; k <= iterations; k++ )
	{
		for ( size_t i = 0; i < this->rows; i++ )
		{
			for ( size_t j = 0; j < this->cols; j++ )
			{
				// Calculate #Neighbours.
				int numNeighbor = this->countNeighbours( i, j );

				// Box ← (( #Neighbours == 3 ) OR ( Cell is alive AND #Neighbours == 2 )).
				this->set( i, j, numNeighbor == 3 || ( this->get( i, j )&& numNeighbor == 2 ) );
			}
		}

		// Swap the reading and writing matrixes.
		this->swap();
	}
}

Matrix::~Matrix()
{
	for ( size_t i = 0; i < this->rows; i++ )
	{
		delete[] this->read[i];
		delete[] this->write[i];
	}
	delete[] this->read;
	delete[] this->write;
}

