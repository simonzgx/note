cmake_minimum_required(VERSION 3.2)

project(test_acl_redis)

include_directories(lib_acl_cpp include)

link_directories(lib)

file(GLOB libs lib/*)

find_package(Threads)

add_executable(main main.cpp)

target_link_libraries(main ${libs})
target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT})

