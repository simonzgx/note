cmake_minimum_required(VERSION 2.8)

project(event)

set(CMAKE_CXX_STANDARD 14)

if (UNIX)
    find_package(Libevent REQUIRED)
    if (Libevent_FOUND)
        include_directories(${LIBEVENT_INCLUDE_DIRS})
    endif ()
else ()
    set(LIBEVENT_INCLUDE_DIRS "H:/prog/libevent-2.1.11-stable")
    include_directories(${LIBEVENT_INCLUDE_DIRS} "H:/prog/libevent-2.1.11-stable/include/")
endif ()

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} ${LIBEVENT_LIBRARIES})