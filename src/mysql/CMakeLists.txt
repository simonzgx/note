cmake_minimum_required(VERSION 3.2)

project(my_mysql_lib)

set(CMAKE_CXX_STANDARD 17)
set(MYSQL_INCLUDE_DIR "C:/Program Files/MySQL/MySQL Server 5.7/include")

add_definitions(-g -O0)

aux_source_directory(. SRC_DIR)

include_directories(${MYSQL_INCLUDE_DIR})

add_executable(mysql main.cpp ${SRC_DIR})