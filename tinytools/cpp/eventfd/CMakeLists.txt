cmake_minimum_required(VERSION 2.8)

project(eventfd)

find_package(Threads)

aux_source_directory(. SRC_DIR)

add_executable(${PROJECT_NAME} ${SRC_DIR})

target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})