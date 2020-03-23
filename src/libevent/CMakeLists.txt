cmake_minimum_required(VERSION 2.8)

project(libevent)

set(CMAKE_CXX_STANDARD 14)

set(SRC_DIR main.cpp)

find_package(libevent REQUIRED)

if(Libevent_FOUND)
    message(hello)

list(APPEND ${SRC_DIR} test.cpp)

add_executable(libevent ${SRC_DIR})

