cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER gcc-9)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-g -Wall -Werror -std=c99 -m64")

file(GLOB source csim.c cachelab.c csim.h)
add_executable(csim ${source})

