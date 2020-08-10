cmake_minimum_required(VERSION 2.8)

project(net)

set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -fno-inline")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

#set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/bin/Release/)
SET(CMAKE_BUILD_TYPE "Debug")

aux_source_directory(. SRC_DIR)

include_directories(.)

set(CMAKE_CXX_STANDARD 14)

add_executable(${PROJECT_NAME} ${SRC_DIR})