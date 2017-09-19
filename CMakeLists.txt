cmake_minimum_required(VERSION 3.5)
project(TextSim)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -std=c++11 -msse4.1 -pthread")
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(PROJECT_TEST_DIR ${PROJECT_SOURCE_DIR}/test)
set(FASTPFORINCLUDE ${PROJECT_SOURCE_DIR}/lib/FastPFor/headers)
set(FASTPFORLIB ${PROJECT_SOURCE_DIR}/lib/FastPFor/libFastPFor.a)
set(CMPHINCLUDE ${PROJECT_SOURCE_DIR}/lib/cmph-2.0/include)
set(CMPHLIB ${PROJECT_SOURCE_DIR}/lib/cmph-2.0/lib/libcmph.so)

file(GLOB SOURCES ${PROJECT_SRC_DIR}/*.cpp)

include_directories(${PROJECT_INCLUDE_DIR})
include_directories(${PROJECT_SRC_DIR})
include_directories(${PROJECT_TEST_DIR})

add_library(libcmph SHARED IMPORTED)
link_directories(${CMPHLIB})
set_target_properties(libcmph PROPERTIES
        IMPORTED_LOCATION ${CMPHLIB})

include_directories(${CMPHINCLUDE})
include_directories(${FASTPFORINCLUDE})

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${CMPHLIB})
target_link_libraries(${PROJECT_NAME} ${FASTPFORLIB})