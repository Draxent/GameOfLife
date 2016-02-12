#include <chrono>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <new>
#include <string>
#include <bitset>
#include <math.h>

using namespace std;
using namespace std::chrono;

#define BITSET_SIZE 64

size_t borderSide, side, numCells, numBlocks, indexBorderSide, indexSide, indexRM;
int offsetBorderSide, offsetRM;
bitset<BITSET_SIZE>* MREAD;
bitset<BITSET_SIZE>* MWRITE;

int roundMultiple( int numToRound, int multiple ) 
{
	return ((numToRound + multiple - 1) / multiple) * multiple;
}

void copyBorder()
{	
	// Fill the bottom border
	for ( int i = 0; i < indexRM; i++ )
		for ( int j = 0; j < BITSET_SIZE; j++ )
			MREAD[numBlocks - indexSide + i][j] = MREAD[indexSide + i][j];
	MREAD[numBlocks - indexSide + indexBorderSide][offsetBorderSide] = MREAD[indexSide][1];	
	MREAD[numBlocks - indexSide][0] = MREAD[indexSide + indexRM][offsetRM];
	
	// Fill the top border
	for ( int i = 0; i < indexRM; i++ )
		for ( int j = 0; j < BITSET_SIZE; j++ )
			MREAD[i][j] = MREAD[numBlocks - 2*indexSide + i][j];
	MREAD[indexBorderSide][offsetBorderSide] = MREAD[numBlocks - 2*indexSide][1];	
	MREAD[0][0] = MREAD[numBlocks - 2*indexSide + indexRM][offsetRM];
	
	// Fill left & right borders
	for ( size_t i = 1; i <= borderSide - 2; i++ )
	{
		MREAD[i*indexSide][0] = MREAD[i*indexSide + indexRM - 1][offsetRM];
		MREAD[i*indexSide + indexBorderSide - 1][offsetBorderSide + 1] = MREAD[i*indexSide][1];
	}
}

int countNeighbors( size_t index, int offset )
{
	int c = 0;
	
	if ( offset == 0 )
	{
		c += MREAD[index - indexSide - 1][BITSET_SIZE - 1];
		c += MREAD[index - indexSide][0];
		c += MREAD[index - indexSide][1];
		
		c += MREAD[index - 1][BITSET_SIZE - 1];
		c += MREAD[index][1];
		
		c += MREAD[index + indexSide - 1][BITSET_SIZE - 1];
		c += MREAD[index + indexSide][0];
		c += MREAD[index + indexSide][1];
	}
	else if ( offset == BITSET_SIZE - 1 )
	{
		c += MREAD[index - indexSide][BITSET_SIZE - 2];
		c += MREAD[index - indexSide][BITSET_SIZE - 1];
		c += MREAD[index - indexSide + 1][0];
		
		c += MREAD[index][BITSET_SIZE - 2];
		c += MREAD[index + 1][0];
		
		c += MREAD[index + indexSide][BITSET_SIZE - 2];
		c += MREAD[index + indexSide][BITSET_SIZE - 1];
		c += MREAD[index + indexSide + 1][0];
	}
	else
	{
		c += MREAD[index - indexSide][offset - 1];
		c += MREAD[index - indexSide][offset];
		c += MREAD[index - indexSide][offset + 1];
		
		c += MREAD[index][offset - 1];
		c += MREAD[index][offset + 1];
		
		c += MREAD[index + indexSide][offset - 1];
		c += MREAD[index + indexSide][offset];
		c += MREAD[index + indexSide][offset + 1];
	}
	
    return c;
}

void print_matrix( string name, bitset<BITSET_SIZE>* M, bool border = false )
{
	int add = border ? 0 : 1;
	cout << name << ":" << endl;
	for ( size_t i = add; i < side - add; i++ )
	{
		size_t pos = i * side;
		size_t index = (pos + add) / BITSET_SIZE;
		int offset =  (pos + add) % BITSET_SIZE;
		cout << M[index][offset] ? "1" : "0";
		
		for ( size_t j = 1 + add; j < side - add; j++ )
		{
			index = (pos + j) / BITSET_SIZE;
			offset =  (pos + j) % BITSET_SIZE;
			cout << " " << M[index][offset] ? "1" : "0";
		}
		cout << endl;
	}
	cout << endl;
}

int main( int argc, char* argv[] )
{
	high_resolution_clock::time_point t1, t2;
	
	if ( argc != 3 )
	{
		cerr << "Usage: " << argv[0] << " side num_iterations" << endl;
		return 1;
	}

	// Enlarge the matrix with a border in order to favor locality
	/*  _ _ _ _ _
	 * |_|_|_|_|_|
	 * |_|x|x|x|_|
	 * |_|x|x|x|_|
	 * |_|x|x|x|_|
	 * |_|_|_|_|_|
	 */ 
	
	int realSide = atoi( argv[1] );
	borderSide =  realSide + 2;
	side = roundMultiple( borderSide, BITSET_SIZE );
	numCells = side * borderSide;
	numBlocks = ceil(numCells / BITSET_SIZE);
	int num_iterations = atoi( argv[2] );
	indexSide = side / BITSET_SIZE;
	indexBorderSide = borderSide / BITSET_SIZE;
	offsetBorderSide = borderSide % BITSET_SIZE;
	indexRM = ( realSide + 1 ) / BITSET_SIZE;
	offsetRM = ( realSide + 1 ) % BITSET_SIZE;
	
	// Initialize random seed
	srand ( time(NULL) );
	
	try
	{	
		MREAD = new bitset<BITSET_SIZE>[ numBlocks ];
		MWRITE = new bitset<BITSET_SIZE>[ numBlocks ];
	}
	catch( std::bad_alloc& badAlloc )
	{
		std::cerr << "Error: not enough memory, reduce side value." << std::endl;
		return 1;
	}
	
	// Fill the matrix width random values
	for ( size_t i = 0; i < numBlocks; i++ )
		for ( int j = 0; j <  BITSET_SIZE; j++ )
			MREAD[i][j] = (rand() % 100 + 1 > 50) ? 1 : 0;
	
#if DEBUG
	print_matrix( "INIT", MREAD );
#endif // DEBUG
	
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	
	t1 = high_resolution_clock::now();
	
	for ( int k = 0; k < num_iterations; k++ )
	{
		copyBorder();
		
#if DEBUG
		print_matrix( "STEP " + to_string((long long) k), MREAD, true );
#endif // DEBUG
		
		for ( size_t index = indexSide; index < numBlocks - 2*indexSide + indexRM; index++ )
		{		
			for ( int offset = 0; offset < BITSET_SIZE; offset++ )
			{
				int numNeighbors = countNeighbors( index, offset );
				MWRITE[index][offset] = ( numNeighbors == 3 || ( MREAD[index][offset] && numNeighbors == 2 ) );
			}
		}
		
		// Swap
		bitset<BITSET_SIZE>* tmp = MREAD;
		MREAD  = MWRITE;
		MWRITE = tmp;
	}

	t2 = high_resolution_clock::now();
	
#if DEBUG
	print_matrix( "RESULT", MREAD );
#endif // DEBUG
	
	auto duration = duration_cast<microseconds>(t2 - t1).count();
	cout << duration << endl;

	return 0;
}