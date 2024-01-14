cmake_minimum_required(VERSION 3.26)
project(PNBA_parallel)

set(CMAKE_CXX_STANDARD 17)

add_executable(PNBA_parallel main.cpp
    Node.cpp Path.cpp Astar_search.cpp)
