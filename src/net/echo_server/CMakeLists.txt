cmake_minimum_required(VERSION 2.8)

project(echo-server)

set(CMAKE_CXX_STANDARD 14)

aux_source_directory(${CMAKE_SOURCE_DIR}/src SRC_DIR)

find_package(Libevent)

if (NOT Libevent_FOUND)
    set(LIBEVENT_INCLUDE_DIRS "D:/prog/libevent-2.1.11-stable/include")
    message(LibEvent NOT FOUND)
endif ()

include_directories(${CMAKE_SOURCE_DIR}/include ${LIBEVENT_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SRC_DIR})

target_link_libraries(${PROJECT_NAME} event)

