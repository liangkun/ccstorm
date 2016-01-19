# Copyright (c) 2015 SHUMEI Inc. All Rights Reserved.
# Authors: Liang Kun <liangkun@ishumei.com>

CMAKE_MINIMUM_REQUIRED(VERSION 3.3)
SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

PROJECT(Storm CXX)

set(LIB_MAJOR_VERSION "0")
set(LIB_MINOR_VERSION "9")
set(LIB_PATCH_VERSION "0")
set(LIB_VERSION_STRING "${LIB_MAJOR_VERSION}.${LIB_MINOR_VERSION}.${LIB_PATCH_VERSION}")

# Compile in release with debug info mode by default
SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build Type")
option(STORM_BUILD_TESTS "Build storm unittests." ON)
option(STORM_PROTOCOL_DEBUG_ENABLE "Log every input/output message." OFF)
set(STORM_PROTOCOL_DEBUG "/tmp")

if (STORM_PROTOCOL_DEBUG_ENABLE)
    add_definitions(-DSTORM_PROTOCOL_DEBUG="${STORM_PROTOCOL_DEBUG}")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -Wall -Wextra")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -Wall -Wextra")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")
file(GLOB_RECURSE TEST_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cc")

# Find thrift include/lib/compiler
find_package(Thrift REQUIRED)
include_directories(SYSTEM ${THRIFT_INCLUDE_DIR})
set(LIBS ${LIBS} ${THRIFT_LIBRARIES})

thrift_gen_cpp(${CMAKE_CURRENT_SOURCE_DIR}/src/storm.thrift THRIFT_CC_FILES_LIST THRIFT_GEN_INCLUDE_DIR)
include_directories(${THRIFT_GEN_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(storm ${THRIFT_CC_FILES_LIST} ${SOURCE} ${LIBS})

set(STORM_LIBS storm ${LIBS})
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(STORM_LIBS ${STORM_LIBS} -lpthread -lrt)
endif()

add_executable(adder ${CMAKE_CURRENT_SOURCE_DIR}/bolts/adder.cc)
target_link_libraries(adder ${STORM_LIBS})

add_executable(drpc ${CMAKE_CURRENT_SOURCE_DIR}/tools/drpc.cc)
target_link_libraries(drpc ${STORM_LIBS})

add_executable(jsoneq ${CMAKE_CURRENT_SOURCE_DIR}/tools/jsoneq.cc)
target_link_libraries(jsoneq ${STORM_LIBS})

if(STORM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/googletest/googletest)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/googletest/googletest/include)
    add_executable(run_unit_tests ${TEST_SOURCE})
    target_link_libraries(run_unit_tests storm gtest gtest_main)
    add_test(run_unit_tests run_unit_tests)
endif()
