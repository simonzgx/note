cmake_minimum_required(VERSION 3.10)

project(kafkatest)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -Wno-sign-compare -g -ggdb -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wno-old-style-cast")

find_package(RdKafka REQUIRED)
find_package(Protobuf REQUIRED)
if (NOT Protobuf_FOUND)
    set(Protobuf_LIBRARY /usr/local/lib/libprotobuf.a)
    set(Protobuf_INCLUDE_DIR /usr/local/include)
endif ()

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${RdKafka_INCLUDE_DIR})
include_directories(${Protobuf_INCLUDE_DIR})

aux_source_directory(. SRC_DIR)

add_executable(${PROJECT_NAME} ${SRC_DIR})

target_link_libraries(${PROJECT_NAME} ${RdKafka_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${Protobuf_LIBRARY})