cmake_minimum_required(VERSION 2.8)

project(libevent)

set(CMAKE_CXX_STANDARD 14)

set(SRC_DIR main.cpp)

list(APPEND ${SRC_DIR} test.cpp)

add_executable(libevent ${SRC_DIR})

