cmake_minimum_required(VERSION 3.26.0)
project(engine VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -g -fdeclspec")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse3 -Wall -g -fdeclspec -DSKY_USE_SIMD")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory("engine")
add_subdirectory("sandbox")
add_subdirectory("testbed")
