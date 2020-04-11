cmake_minimum_required(VERSION 2.8)

project(redis_cpp)

set(CMAKE_CXX_STANDARD 14)

set(INCLUDE_DIR include)
aux_source_directory(. SRC_DIR)

list(APPEND INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/redisclient)

find_package(boost)
if (Boost_FOUND)
    list(APPEND INCLUDE_DIR ${Boost_INCLUDE_DIRS})
else ()
    message(SEND_ERROR Boost_NOT_FOUND)
endif ()

include_directories(${INCLUDE_DIR})

add_executable(redis_client main.cpp ${SRC_DIR})

add_subdirectory(src/redisclient)