cmake_minimum_required(VERSION 2.8)

project(main)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")


if(CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-g -std=c++11 -Wno-deprecated)
endif(CMAKE_COMPILER_IS_GNUCXX)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} CUR_DIR_SRCS)

set(EXE_NAME main)
add_executable(${EXE_NAME} ${CUR_DIR_SRCS})

target_link_libraries(${EXE_NAME} pthread)

