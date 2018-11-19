cmake_minimum_required(VERSION 2.8)

#FIND_PACKAGE(lmdb)

#message(STATUS "lmdb_LIBS=${lmdb_LIBS}")

add_executable(lmdbt lmdb_test.cpp)
target_link_libraries(lmdbt lmdb)
