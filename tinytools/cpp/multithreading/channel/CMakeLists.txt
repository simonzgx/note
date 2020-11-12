cmake_minimum_required(VERSION 2.8)

project(channel)

find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIB_INIT})