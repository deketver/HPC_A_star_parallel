cmake_minimum_required(VERSION 3.26)
project(serial)


set(CMAKE_CXX_STANDARD 23)

add_executable(serial main.cpp
        Node.cpp Astar_search.cpp Path.cpp)