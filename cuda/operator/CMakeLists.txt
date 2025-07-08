cmake_minimum_required(VERSION 3.18)

project(operator LANGUAGES CXX CUDA)

add_library(operator STATIC
    warp_reduce.cu
)