cmake_minimum_required(VERSION 3.5)
project(TextSim)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -std=c++11 -msse4.1 -pthread")
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(PROJECT_TEST_DIR ${PROJECT_SOURCE_DIR}/test)

file(GLOB SOURCES ${PROJECT_SRC_DIR}/*.cpp)

include_directories(${PROJECT_INCLUDE_DIR})
include_directories(${PROJECT_SRC_DIR})
include_directories(${PROJECT_TEST_DIR})


add_executable(${PROJECT_NAME} ${SOURCES})