cmake_minimum_required(VERSION 3.1)

project(quote_monitor)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(. SRC_DIR)

file(GLOB INCLUDE_FILE ${CMAKE_SOURCE_DIR}/*.h ${CMAKE_SOURCE_DIR}/*.hpp)


add_executable(monitor ${SRC_DIR} ${INCLUDE_FILE})