cmake_minimum_required(VERSION 3.18)

project(operator)

enable_language(CUDA)

file(GLOB OP_SRC
    ${PROJECT_SOURCE_DIR}/*.cu
    ${PROJECT_SOURCE_DIR}/*.cc
)

set_source_files_properties(${OP_SRC} PROPERTIES LANGUAGE CUDA)
add_library(operator STATIC ${OP_SRC})
target_compile_options(operator PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:
                       -g
                       -G
                       >)
target_include_directories(operator PUBLIC ${PRIMITIVE_HOME})