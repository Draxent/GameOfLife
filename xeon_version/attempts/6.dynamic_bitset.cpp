#include <chrono>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <new>
#include <string>
#include <boost/dynamic_bitset.hpp>

using namespace std;
using namespace std::chrono;
using boost_bitset = boost::dynamic_bitset<unsigned long long>;

size_t side;
size_t numCells;
boost_bitset* MREAD;
boost_bitset* MWRITE;

void copyBorder()
{	
	// Fill the bottom border
	(*MREAD)[numCells - 1] = (*MREAD)[side + 1];
	for ( size_t i = 1; i <= side - 2; i++ )
		(*MREAD)[numCells - side + i] = (*MREAD)[side + i];
	(*MREAD)[numCells - side] = (*MREAD)[2*side - 2];
	
	// Fill the top border
	(*MREAD)[side - 1] = (*MREAD)[numCells - 2*side + 1];
	
	for ( size_t i = 1; i <= side - 2; i++ )
		(*MREAD)[i] = (*MREAD)[numCells - 2*side + i];
	(*MREAD)[0] = (*MREAD)[numCells - side - 2];
	
	// Fill left & right borders
	for ( size_t i = 1; i <= side - 2; i++ )
	{
		(*MREAD)[i*side] = (*MREAD)[(i+1)*side - 2];
		(*MREAD)[(i+1)*side - 1] = (*MREAD)[i*side + 1];
	}
}

int countNeighbors( size_t pos )
{
	int c = 0;
	
	c += (*MREAD)[ pos - side - 1 ];
	c += (*MREAD)[ pos - side ];
	c += (*MREAD)[ pos - side + 1 ];
	
	c += (*MREAD)[ pos - 1 ];
	c += (*MREAD)[ pos + 1 ];

	c += (*MREAD)[ pos + side - 1 ];
	c += (*MREAD)[ pos + side ];
	c += (*MREAD)[ pos + side + 1 ];
	
    return c;
}

void print_matrix( string name, boost_bitset* M, bool border = false )
{
	int add = border ? 0 : 1;
	cout << name << ":" << endl;
	for ( size_t i = add; i < side - add; i++ )
	{
		size_t pos = i * side;
		cout << (*M)[pos + add] ? "1" : "0";
		for ( size_t j = 1 + add; j < side - add; j++ )
			cout << " " << (*M)[pos + j] ? "1" : "0";
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
	side = atoi( argv[1] ) + 2;
	numCells = side * side;
	int num_iterations = atoi( argv[2] );
	
	// Initialize random seed
	srand ( time(NULL) );
	
	try
	{
		MREAD = new boost_bitset( numCells );
		MWRITE = new boost_bitset( numCells );
	}
	catch( std::bad_alloc& badAlloc )
	{
		std::cerr << "Error: not enough memory, reduce side value." << std::endl;
		return 1;
	}
	
	// Fill the matrix width random values
	for ( size_t i = 0; i < numCells; i++ )
		(*MREAD)[i] = (rand() % 100 + 1 > 50) ? 1 : 0;
	
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
			
		for ( size_t pos = side + 1; pos < numCells - side - 1; pos++ )
		{
			int numNeighbors = countNeighbors( pos );
			(*MWRITE)[pos] = ( numNeighbors == 3 || ( (*MREAD)[pos] && numNeighbors == 2 ) );
		}
		
		// Swap
		boost_bitset* tmp = MREAD;
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