#include <chrono>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <new>

using namespace std;
using namespace std::chrono;

size_t side;
size_t numCells;
bool* M;

void copyBorder( bool shifted )
{
	size_t remove = shifted ? (side + 1) : 0;
	int remove2 = shifted ? 1 : 0;
	
	// Fill the bottom border
	M[numCells - 1 - remove] = M[2*side + 2 - remove];
	copy( M + 2*side + 2 - remove, M + 3*side - 1 - remove, M + numCells - side + 2 - remove );
	M[numCells - side + 1 - remove] = M[3*side - 2 - remove];
	
	// Fill the top border
	M[2*side - 1 - remove] = M[numCells - 2*side + 2 - remove];
	copy( M + numCells - 2*side + 2 - remove, M + numCells - side - 1 - remove, M + side + 2 - remove );
	M[side + 1 - remove] = M[numCells - side - 2 - remove];
	
	// Fill left & right borders
	for ( size_t i = 2 - remove2; i <= side - 2 - remove2; i++ )
	{
		M[i*side + 1 - remove2] = M[(i+1)*side - 2 - remove2];
		M[(i+1)*side - 1 - remove2] = M[i*side + 2 - remove2];
	}
}

int countNeighbors( int pos )
{
	int c = 0;
	
	c += M[ pos - side - 1 ];
	c += M[ pos - side ];
	c += M[ pos - side + 1 ];
	
	c += M[ pos - 1 ];
	c += M[ pos + 1 ];

	c += M[ pos + side - 1 ];
	c += M[ pos + side ];
	c += M[ pos + side + 1 ];
		
    return c;
}

void print_matrix( string name, bool* M, bool shifted, bool border = false)
{
	int addLeft = border ? (shifted ? 0 : 1) : (shifted ? 1 : 2);
	int removeRight = border ? (shifted ? 1 : 0) : (shifted ? 2 : 1);
	
	cout << name << ":" << endl;
	for ( size_t i = addLeft; i < side - removeRight; i++ )
	{
		size_t pos = i * side;
		cout << M[pos + addLeft] ? "1" : "0";
		for ( size_t j = 1 + addLeft; j < side - removeRight; j++ )
			cout << " " << M[pos + j] ? "1" : "0";
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
	/*  _ _ _ _ _ _			 	 _ _ _ _ _ _
	 * |_|_|_|_|_|_|			|_|_|_|_|_|_|
	 * |_|_|_|_|_|_|			|_|x|x|x|_|_|
	 * |_|_|x|x|x|_|	-->		|_|x|x|x|_|_|
	 * |_|_|x|x|x|_|			|_|x|x|x|_|_|
	 * |_|_|x|x|x|_|			|_|_|_|_|_|_|
	 * |_|_|_|_|_|_|			|_|_|_|_|_|_|
	 */ 
	side = atoi( argv[1] ) + 3;
	numCells = side * side;
	int num_iterations = atoi( argv[2] );
	
	// Initialize random seed
	srand ( time(NULL) );
	
	try { M = new bool[numCells]; }
	catch( std::bad_alloc& badAlloc )
	{
		std::cerr << "Error: not enough memory, reduce side value." << std::endl;
		return 1;
	}
	
	// Fill the matrix width random values
	for ( size_t i = 0; i < numCells; i++ )
		M[i] = (rand() % 100 + 1 > 50);
	
#if DEBUG
	print_matrix( "INIT", M, false );
#endif // DEBUG
	
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	
	t1 = high_resolution_clock::now();
	
	bool shifted = false; 
	for ( int k = 0; k < num_iterations; k++ )
	{
		copyBorder( shifted );
		
#if DEBUG
		print_matrix( "STEP " + to_string((long long) k), M, shifted, true );
#endif // DEBUG
		
		if ( shifted )
		{
			for ( size_t pos = numCells - 2*(side + 1); pos > side + 1; pos-- )
			{
				int numNeighbors = countNeighbors( pos );
				M[pos + side + 1] = ( numNeighbors == 3 || ( M[pos] && numNeighbors == 2 ) );
			}
		}
		else
		{
			for ( size_t pos = 2*(side + 1); pos < numCells - side - 1; pos++ )
			{
				int numNeighbors = countNeighbors( pos );
				M[pos - side - 1] = ( numNeighbors == 3 || ( M[pos] && numNeighbors == 2 ) );
			}
		}
		
		shifted = !shifted;
	}

	t2 = high_resolution_clock::now();
	
#if DEBUG
	print_matrix( "RESULT", M, shifted );
#endif // DEBUG
	
	auto duration = duration_cast<microseconds>(t2 - t1).count();
	cout << duration << endl;

	return 0;
}