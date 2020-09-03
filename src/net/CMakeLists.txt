cmake_minimum_required(VERSION 3.10)

project(net)

set(CMAKE_CXX_STANDARD 14)

aux_source_directory(src SRC_DIR)

include_directories(include)

add_library(net ${SRC_DIR})