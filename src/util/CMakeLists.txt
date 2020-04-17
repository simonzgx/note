cmake_minimum_required(VERSION 2.8)

project(test)

aux_source_directory(. SRC_DIR)

add_executable(main ${SRC_DIR})