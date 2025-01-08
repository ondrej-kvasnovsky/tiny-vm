cmake_minimum_required(VERSION 3.30)
project(untitled)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        compare_and_swap
        compare_and_swap.cpp
)

add_executable(
        memory_barriers
        memory_barriers.cpp
)
