#include <iostream>
#include <vector>
#include <mutex>
#include <cmath>
#include <ff/farm.hpp>
#include <ff/pipeline.hpp>

std::mutex mtx;

struct Task_t
{
	Task_t ( size_t start, size_t end) : start(start), end(end) { }
	const size_t start, end;
};

class Grid
{
public:
	int* READ;
	int* WRITE;
	Grid( int num_cells ) : num_cells(num_cells)
	{
		READ = new int[num_cells];
		WRITE = new int[num_cells];
		for ( size_t i = 0; i < num_cells; i++ )
			READ[i] = 0;
	}
	size_t size()
	{
		return num_cells;
	}
	void swap()
	{
		int* tmp = this->READ;
		this->READ  = this->WRITE;
		this->WRITE = tmp;
	}
	void print()
	{
		std::cout << "GRID = { " << this->READ[0];
		for (int i = 1; i < this->num_cells; i++)
			std::cout << ", " << this->READ[i];
		std::cout << "}" << std::endl;
	}

private:
	const size_t num_cells;
};


class Master: public ff::ff_node_t<Task_t>
{
public:
	Master( ff::ff_loadbalancer* const lb, Grid* g, unsigned int iterations, size_t start, size_t end, size_t chunk_size, unsigned long num_tasks )
			: lb(lb), g(g), iterations(iterations), start(start), end(end),
			  chunk_size(chunk_size), num_tasks(num_tasks)
	{
		this->completed_iterations = 0;
		this->start_chunk = 0;
		this->end_chunk = start;
		this->counter = 0;
	}
	void send_work()
	{
		this->start_chunk = 0;
		this->end_chunk = start;
		while ( this->end_chunk < this->end )
		{
			// Create new Task.
			this->start_chunk = this->end_chunk;
			this->end_chunk = std::min( this->start_chunk + this->chunk_size, this->end );
			Task_t *task = new Task_t( this->start_chunk, this->end_chunk );
			// Send Task to the next Worker.
			ff_send_out( task );
		}
	}
	Task_t* svc( Task_t* task )
	{
		if ( task == nullptr )
		{
			// First iteration
			send_work();
			return GO_ON;
		}
		else
		{
			// Increment Task counter and delete Task.
			this->counter++;
			delete( task );

			// If the Task counter is equal to the number of task sent, we complete the iteration.
			if ( this->counter == this->num_tasks )
			{
				// Reset counter.
				this->counter = 0;

				// Increment the number of completed iterations.
				this->completed_iterations++;

				g->swap();
				g->print();
				// 	end_generation( g, this->completed_iterations );

				// Send EOS if we completed all the iterations.
				if ( this->completed_iterations == this->iterations )
					return EOS;
				else // We go on with the computation of the next generation.
				{
					this->send_work();
					return GO_ON;
				}
			}
			else return GO_ON;
		}
	}

private:
	ff::ff_loadbalancer* const lb;
	const unsigned int iterations;
	const size_t start, end, chunk_size;
	const unsigned long num_tasks;
	size_t start_chunk, end_chunk;
	unsigned long counter;
	int completed_iterations;
	Grid* g;
};

class Worker : public ff::ff_node_t<Task_t>
{
public:
	Worker( int id, Grid* g) : id(id), g(g) { }

	Task_t* svc( Task_t* task )
	{
		mtx.lock();
		std::cout << "Worker " << this->id << " compute [" << task->start << ", " << task->end << ")." << std::endl; fflush( stdout );
		mtx.unlock();
		for ( size_t pos = task->start; pos < task->end; pos++ )
			g->WRITE[pos] = g->READ[pos] + pos;
		return task;
	}
private:
	int id;
	Grid* g;
};

int main()
{
	unsigned int iterations = 3;
	unsigned int nw = 5;
	Grid* g = new Grid( 30 );
	g->print();

	size_t start = 0;
	size_t end = g->size();
	size_t chunk_size = 2;
	unsigned long num_tasks = (unsigned long) ceil( g->size() / chunk_size );
	std::cout << "num_tasks: " << num_tasks << std::endl;

	std::vector<std::unique_ptr<ff::ff_node>> workers;
	for ( int t = 0; t < nw; t++ )
		workers.push_back( ff::make_unique<Worker>( t, g ) );
	// Create the Farm.
	ff::ff_Farm<> farm( std::move( workers ) );

	// Removes the default collector.
	farm.remove_collector();

	// The scheduler gets in input the internal load-balancer.
	Master master( farm.getlb(), g, iterations, start, end, chunk_size, num_tasks );
	farm.add_emitter( master );

	// Adds feedback channels between each worker and the scheduler.
	farm.wrap_around();

	if ( farm.run_and_wait_end() < 0 )
		ff::error("running farm");
	return 0;
}