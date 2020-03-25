cmake_minimum_required(VERSION 2.8)

project(libevent-test)

set(CMAKE_CXX_STANDARD 14)

set(SRC_DIR main.cpp)

set(INCLUDE_DIR
        /Users/simon/git/libevent/include)

message(${INCLUDE_DIR})

find_package(libevent REQUIRED)

if (NOT libevent_FOUND)
    message(LibEvent NOT FOUND)
else ()
    list(APPEND ${LIBEVENT_INCLUDE_DIRS})
    target_link_libraries(libevent)
endif ()

list(APPEND ${SRC_DIR} test.cpp)

include_directories(${INCLUDE_DIR})

add_executable(libevent-test ${SRC_DIR})

