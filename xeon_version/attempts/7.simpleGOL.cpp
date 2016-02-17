#include <chrono>
#include <iostream>

using namespace std;
using namespace std::chrono;

int main( int argc, char* argv[] )
{
	high_resolution_clock::time_point t1, t2;

	if ( argc != 3 )
	{
		cerr << "Usage: " << argv[0] << " side num_iterations" << endl;
		return 1;
	}

	int side = atoi( argv[1] ) + 2;
	int numCells = side * side;
	int num_iterations = atoi( argv[2] );
	bool* MREAD = new bool[numCells];
	bool* MWRITE = new bool[numCells];

	// Initialize random seed
	srand ( 1 );

	// Fill the matrix width random values
	for ( int i = 0; i < numCells; i++ )
		MREAD[i] = ( rand() > 0.5 );

	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////

	t1 = high_resolution_clock::now();

	int start = side + 1;
	int end = numCells - side - 1;

	for ( int k = 0; k < num_iterations; k++ )
	{
		int pos_up = 1, pos_down = 2*side + 1;
		for ( int pos = start; pos < end; pos++, pos_up++, pos_down++  )
		{
			int numNeighbors = MREAD[ pos_up - 1 ] +
							   MREAD[ pos_up ] +
							   MREAD[ pos_up + 1 ] +
							   MREAD[ pos - 1 ] +
							   MREAD[ pos + 1 ] +
							   MREAD[ pos_down - 1 ] +
							   MREAD[ pos_down ] +
							   MREAD[ pos_down + 1 ];
			MWRITE[pos] = ( numNeighbors == 3 || ( MREAD[pos] && numNeighbors == 2 ) );
		}

		// Swap
		bool* tmp = MREAD;
		MREAD  = MWRITE;
		MWRITE = tmp;
	}

	t2 = high_resolution_clock::now();

	auto duration = duration_cast<microseconds>(t2 - t1).count();
	cout << duration << endl;

	return 0;
}
