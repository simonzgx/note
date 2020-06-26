#!/bin/bash
TYPE=$1

rm CMakeCache.txt
rm -rf CMakeFiles/
cmake -f CMakeLists.txt -DCMAKE_BUILD_TYPE=$TYPE
make -j4

