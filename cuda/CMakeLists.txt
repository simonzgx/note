cmake_minimum_required(VERSION 3.18)

project(test LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-O0 -g)

enable_language(CUDA)

find_package(CUDA REQUIRED)

set(CUDA_ARCHITECTURES "75;80;86" CACHE STRING "CUDA architectures to compile for")

add_executable(test warp_reduce.cu)