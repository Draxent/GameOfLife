cmake_minimum_required(VERSION 3.3)
project(GameOfLife)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories("~/fastflow")
set(SOURCE_FILES src/main_thread.cpp include/specialized_functions.h include/grid.h include/master.h include/program_options.h include/worker.h src/specialized_functions.cpp src/grid.cpp src/master.cpp src/program_options.cpp src/worker.cpp include/spinning_barrier.h src/spinning_barrier.cpp  src/main_ff.cpp include/grid_vect.h src/grid_vect.cpp include/shared_functions.h src/shared_functions.cpp include/worker_vect.h src/worker_vect.cpp include/matrix.h src/matrix.cpp)
add_executable(GameOfLife ${SOURCE_FILES})

cmake_minimum_required(VERSION 3.3)
project(GameOfLife)