/**
 *	@file specialized_functions.cpp
 *  @brief Implementation of functions needed to development different methodologies.
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

#include "../include/specialized_functions.h"

#if DEBUG
// mutex for critical section
std::mutex mtx;
#endif // DEBUG

void compute_generation( Grid* g, size_t start, size_t end )
{
	size_t pos_top = start - g->width(), pos_bottom = start + g->width();

	for ( size_t pos = start; pos < end; pos++, pos_top++, pos_bottom++ )
	{
		// Calculate #Neighbours.
		int numNeighbor = g->countNeighbours( pos, pos_top, pos_bottom );
		// Box ← (( #Neighbours == 3 ) OR ( Cell is alive AND #Neighbours == 2 )).
		g->Write[pos] = ( numNeighbor == 3 || ( g->Read[pos] && numNeighbor == 2 ) );
	}
}

void compute_generation_vect( Grid* g, int* numsNeighbours, size_t start, size_t end )
{
	size_t index = start, index_top = start - g->width(), index_bottom = start + g->width();
	for ( ; index + VLEN < end; index += VLEN, index_top += VLEN, index_bottom += VLEN )
	{
		// Save the computation of the neighbours counting into the numsNeighbours array.
		numsNeighbours[0:VLEN] = g->countNeighbours( index, index_top, index_bottom, __sec_implicit_index(0) );
		// Box ← (( #Neighbours == 3 ) OR ( Cell is alive AND #Neighbours == 2 )) in vector notations.
		g->Write[index:VLEN] = ( numsNeighbours[0:VLEN] == 3 || ( g->Read[index:VLEN] && numsNeighbours[0:VLEN] == 2 ) );
	}
	// Compute normally the last piece that does not fill the numsNeighbours array.
	for ( ; index < end; index++, index_top++, index_bottom++ )
	{
		// Calculate #Neighbours.
		int numNeighbor = g->countNeighbours( index, index_top, index_bottom );
		// Box ← (( #Neighbours == 3 ) OR ( Cell is alive AND #Neighbours == 2 )).
		g->Write[index] = ( numNeighbor == 3 || ( g->Read[index] && numNeighbor == 2 ) );
	}
}

void thread_body( int id, Grid* g, size_t start, size_t end, unsigned int iterations, bool vectorization, long* copyborder_time, SpinningBarrier* barrier )
{
#if DEBUG
	mtx.lock();
	std::cout << "Thread " << id << " got range [" << start << "," << end << ")" << std::endl;
	fflush( stdout );
	mtx.unlock();
#endif // DEBUG
	int* numsNeighbours = NULL;
	if ( vectorization )
		numsNeighbours = new int[VLEN];

	for ( unsigned int k = 1; k <= iterations; k++ )
	{
		// The working size has to be significant in order to vectorized the thread_body function.
		if ( vectorization && ( end - start >= VLEN ) )
			compute_generation_vect( g, numsNeighbours, start, end );
		else
			compute_generation( g, start, end );

		// if we are computing the sequential or it is the last thread we compute the end_generation function.
		if ( barrier == NULL || barrier->is_last_thread() )
		{
			*copyborder_time = *copyborder_time + end_generation( g, k );
			if ( barrier != NULL) barrier->notify_all();
		}
	}

	if ( vectorization )
		delete[] numsNeighbours;
}