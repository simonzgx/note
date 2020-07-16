cmake_minimum_required(VERSION 2.8)

project(net)

aux_source_directory(. SRC_DIR)

include_directories(.)

add_executable(${PROJECT_NAME} ${SRC_DIR})