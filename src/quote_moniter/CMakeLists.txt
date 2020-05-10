cmake_minimum_required(VERSION 3.1)

project(quote_monitor)

aux_source_directory(. SRC_DIR )

add_executable(monitor ${SRC_DIR})