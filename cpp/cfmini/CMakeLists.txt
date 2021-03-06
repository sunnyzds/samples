project(gflags_demo)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE on)

find_package(Protobuf)
message(STATUS "PROTOBUF_INCLUDE_DIR=${PROTOBUF_INCLUDE_DIR}")
message(STATUS "CAFFE_PROTO_SRCS=${CAFFE_PROTO_SRCS}")
message(STATUS "CAFFE_PROTO_HDRS=${CAFFE_PROTO_HDRS}")
message(STATUS "PROTOBUF_LIBRARIES=${PROTOBUF_LIBRARIES}")

list(APPEND LINKER_LIBS glog gflags)
message(STATUS "LINKER_LIBS=${LINKER_LIBS}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
                          
set(cfmini_SRCS
    src/cfmini.cpp
    src/common.cpp
    src/io.cpp
)

set(proto_SRCS
    src/proto/caffe.proto
)

if(PROTOBUF_FOUND)
    include_directories(${PROTOBUF_INCLUDE_DIR})
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    protobuf_generate_cpp(CAFFE_PROTO_SRCS CAFFE_PROTO_HDRS ${proto_SRCS})
    add_executable(cfmini ${cfmini_SRCS} ${CAFFE_PROTO_SRCS} ${CAFFE_PROTO_HDRS})
    target_link_libraries(cfmini ${LINKER_LIBS} ${PROTOBUF_LIBRARIES})
else()
    message(WARNING "Protobuf not found, caffe model convert tool won't be built")
endif()
