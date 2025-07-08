cmake_minimum_required(VERSION 3.18)

project(toy LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-O0 -g)

enable_language(CUDA)

find_package(CUDA REQUIRED)

set(CUDA_ARCHITECTURES "75;80;86" CACHE STRING "CUDA architectures to compile for")

include_directories(${CMAKE_SOURCE_DIR}/operator)

add_subdirectory(${CMAKE_SOURCE_DIR}/test)
