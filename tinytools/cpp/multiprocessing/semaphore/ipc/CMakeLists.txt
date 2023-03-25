cmake_minimum_required(VERSION 3.14)

project(ipc VERSION 1.0 LANGUAGES CXX)

include_directories(${PROJECT_SOURCE_DIR}/include)
set(CMAKE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

add_subdirectory(src)