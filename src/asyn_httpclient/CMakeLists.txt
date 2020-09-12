cmake_minimum_required(VERSION 2.8)

project(httpclient)

set(CMAKE_CXX_STANDARD 14)

find_package(CURL)

aux_source_directory(${CMAKE_SOURCE_DIR}/src SRC_DIR)

add_executable(${PROJECT_NAME} ${SRC_DIR} main.cpp)

target_link_libraries(${PROJECT_NAME} curl)

