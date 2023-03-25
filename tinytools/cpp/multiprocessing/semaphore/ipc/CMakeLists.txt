cmake_minimum_required(VERSION 3.14)

project(ipc VERSION 1.0 LANGUAGES CXX)

include_directories(${PROJECT_SOURCE_DIR}/include)
set(CMAKE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-g")


add_subdirectory(src)