cmake_minimum_required(VERSION 2.8)

project(hello C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp")

FIND_PACKAGE(Boost COMPONENTS python)
FIND_PACKAGE(PythonInterp)
FIND_PACKAGE(PythonLibs)

message(STATUS "Boost_INCLUDE_DIR = ${Boost_INCLUDE_DIR}")
message(STATUS "PYTHON_INCLUDE_DIR = ${PYTHON_INCLUDE_DIR}")

include_directories(SYSTEM ${Boost_INCLUDE_DIR})
include_directories(SYSTEM ${PYTHON_INCLUDE_DIR})
include_directories(/include
		    ./ncnn/include
	            ${CMAKE_CURRENT_LIST_DIR}/src)

link_directories(${CMAKE_CURRENT_LIST_DIR}/ncnn/lib)

#list(APPEND LINKER_LIBS opencv_videoio opencv_video opencv_imgcodecs opencv_core opencv_highgui opencv_imgproc ncnn)
#list(APPEND LINKER_LIBS opencv_video opencv_core opencv_highgui opencv_imgproc ncnn)

list(APPEND LINKER_LIBS opencv_video opencv_core opencv_highgui opencv_imgproc)

#file(GLOB SRC ${CMAKE_CURRENT_LIST_DIR}/src/*.h
#              ${CMAKE_CURRENT_LIST_DIR}/src/featuredb.cpp
#	      			${CMAKE_CURRENT_LIST_DIR}/src/mobilefacenet.cpp
#	      			${CMAKE_CURRENT_LIST_DIR}/src/mtcnn.cpp)

file(GLOB SRC ${CMAKE_CURRENT_LIST_DIR}/src/*.h
              ${CMAKE_CURRENT_LIST_DIR}/hello.cpp)

#file(GLOB BINSRC ${CMAKE_CURRENT_LIST_DIR}/src/tcpsender.cpp)
#add_executable(tcpsender ${SRC} ${BINSRC})
#set_target_properties(tcpsender PROPERTIES PREFIX "")
#target_link_libraries(tcpsender ${LINKER_LIBS} pthread)

#file(GLOB LIBSRC)
add_library(hello SHARED ${SRC})
set_target_properties(hello PROPERTIES PREFIX "")

message(STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}")
message(STATUS "PYTHON_LIBRARIES = ${PYTHON_LIBRARIES}")

target_link_libraries(hello ${LINKER_LIBS} ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
