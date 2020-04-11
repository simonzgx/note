cmake_minimum_required(VERSION 2.8)

project(libevent-test)

set(CMAKE_CXX_STANDARD 14)

set(SRC_DIR main.cpp)

set(INCLUDE_DIR include)

list(APPEND ${SRC_DIR} test.cpp)

add_executable(libevent-test ${SRC_DIR})
find_package(libevent REQUIRED)

if (NOT libevent_FOUND)
    message(LibEvent NOT FOUND)
    include_directories(${INCLUDE_DIR})
else ()
    list(APPEND INCLUDE_DIR ${LIBEVENT_INCLUDE_DIRS})
    message(${INCLUDE_DIR})
    include_directories(${INCLUDE_DIR})
    link_libraries(libevent-test libevent)
endif ()

