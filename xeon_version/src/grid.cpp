/**
 *	@file grid.cpp
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


#include "../include/grid.h"

Grid::Grid( size_t height, size_t width )
{
	// Initialize private variables
	this->rows = height + 2;
	this->cols = width + 2;
	this->numCells = this->rows * this->cols;
	this->Read = NULL;
	this->Write = NULL;

	// Allocate the two Grides
	this->allocate();
}

void Grid::init( unsigned int seed )
{
	// Initialize random seed
	srand((seed == 0) ? time(NULL) : seed);

	// Fill the matrix width random values
	for (size_t i = 0; i < this->numCells; i++)
		this->Read[i] = (rand() > RAND_MAX_HALF);
}

void Grid::init_vect( unsigned int seed )
{
	// Initialize random seed
	srand48( ( seed == 0 ) ? time(NULL) : seed );

	// Fill the Grid width random values
	for ( size_t i = 0; i < this->numCells; i += VLEN )
		this->Read[i:VLEN] = ( drand48() > 0.5 );
}


size_t Grid::width() const
{
	return this->cols;
}

size_t Grid::height() const
{
	return this->rows;
}

size_t Grid::size() const
{
	return this->numCells;
}

void Grid::copyBorder()
{
	// Fill the bottom border
	this->Read[numCells - 1] = this->Read[this->cols + 1];
	std::copy( this->Read + this->cols + 1, this->Read + 2*this->cols - 1, this->Read + numCells - this->cols + 1 );
	this->Read[numCells - this->cols] = this->Read[2*this->cols - 2];

	// Fill the top border
	this->Read[this->cols - 1] = this->Read[numCells - 2*this->cols + 1];
	std::copy( this->Read + numCells - 2*this->cols + 1, this->Read + numCells - this->cols - 1, this->Read + 1 );
	this->Read[0] = this->Read[numCells - this->cols - 2];

	// Fill left & right borders
	for ( size_t i = 1; i <= this->rows - 2; i++ )
	{
		this->Read[i*this->cols] = this->Read[(i+1)*this->cols - 2];
		this->Read[(i+1)*this->cols - 1] = this->Read[i*this->cols + 1];
	}
}

void Grid::swap()
{
	bool* tmp = this->Read;
	this->Read  = this->Write;
	this->Write = tmp;
}

void Grid::print( const char* msg, bool border )
{
	size_t add = border ? 0 : 1;
	std::cout << msg << " Grid (rows: " << (border ? this->rows : (this->rows - 2)) << ", columns: " << (border ? this->cols : (this->cols - 2)) << ") :" << std::endl;
	for ( size_t i = add; i < this->rows - add; i++ )
	{
		size_t pos = i * this->cols;
		std::cout << this->Read[pos + add] ? "1" : "0";
		for ( size_t j = 1 + add; j < this->cols - add; j++ )
			std::cout << " " << this->Read[pos + j] ? "1" : "0";
		std::cout << std::endl;
	}
}

void Grid::allocate()
{
	try
	{
		this->Read = new bool[numCells];
		this->Write = new bool[numCells];
	}
	catch( std::bad_alloc& badAlloc )
	{
		std::cerr << "Error: not enough memory, reduce side value." << std::endl;
		exit( 1 );
	}
}

Grid::~Grid()
{
	delete[] this->Read;
	delete[] this->Write;
}