project(gflags_demo)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE on)

#FIND_PACKAGE(glfags REQUIRED)

#include_directories(SYSTEM ${GFLAGS_INCLUDE_DIR})

#include_directories("/home/amcool/local/gflags-2.1.1/include")
#link_directories("/home/amcool/local/gflags-2.1.1/lib")

add_executable(gflags_demo gflags_demo.cpp)
target_link_libraries(gflags_demo gflags pthread)
