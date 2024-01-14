cmake_minimum_required(VERSION 3.26)
project(HDA)

set(CMAKE_CXX_STANDARD 17)

add_executable(HDA main.cpp
    Node.cpp Path.cpp Astar_search.cpp)
