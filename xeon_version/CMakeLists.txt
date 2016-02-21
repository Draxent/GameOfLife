cmake_minimum_required(VERSION 3.3)
project(GameOfLife)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories("~/fastflow")
set(SOURCE_FILES src/main_thread.cpp include/grid.h include/master.h include/program_options.h include/worker.h src/grid.cpp src/master.cpp src/program_options.cpp src/worker.cpp src/main_ff.cpp include/shared_functions.h src/shared_functions.cpp include/matrix.h src/matrix.cpp include/task.h)
add_executable(GameOfLife ${SOURCE_FILES})

cmake_minimum_required(VERSION 3.3)
project(GameOfLife)