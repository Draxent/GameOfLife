cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project( game_of_life )

find_package( OpenCV REQUIRED )
find_package( Threads REQUIRED )
find_package( X11 REQUIRED )

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(GCC_OPTIMIZER "-O3")
set(OPENCV "-D OPENCV")

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_definitions(${GCC_OPTIMIZER})
add_definitions(${OPENCV})

add_executable(game_of_life ${SOURCES})

target_compile_features(game_of_life PRIVATE cxx_range_for)
target_link_libraries( game_of_life ${X11_LIBRARIES} )
target_link_libraries( game_of_life ${OpenCV_LIBS} )
target_link_libraries (game_of_life Threads::Threads)
