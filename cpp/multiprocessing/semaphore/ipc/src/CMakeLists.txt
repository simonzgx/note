CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-g")

add_subdirectory(alice)
add_subdirectory(bob)

set(SRC_LIST ./main/main.cc)
include_directories(./)