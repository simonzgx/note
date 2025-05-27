cmake_minimum_required(VERSION 2.8)

project(rw_lock)

set(CMAKE_CXX_STANDARD 14)

link_libraries(pthread)

add_executable(rw_lock main.cpp)