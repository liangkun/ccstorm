# Copyright (c) 2015 Shumei Inc. All Rights Reserved.
# Authors: Liang Kun <liangkun@ishumei.com>

CMAKE_MINIMUM_REQUIRED(VERSION 3.3)
SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

PROJECT(CCStorm CXX)

set(LIB_MAJOR_VERSION "0")
set(LIB_MINOR_VERSION "9")
set(LIB_PATCH_VERSION "0")
set(LIB_VERSION_STRING "${LIB_MAJOR_VERSION}.${LIB_MINOR_VERSION}.${LIB_PATCH_VERSION}")

# Compile in release with debug info mode by default
SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build Type")

option(CCSTORM_BUILD_TESTS "Build ccstorm unittests." ON)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -Wall -Wextra")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -Wall -Wextra")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
endif()

# Find thrift include/lib/compiler
find_package(Thrift REQUIRED)
include_directories(SYSTEM ${THRIFT_INCLUDE_DIR})
set(LIBS ${LIBS} ${THRIFT_LIBRARIES})
thrift_gen_cpp(${CMAKE_CURRENT_SOURCE_DIR}/src/storm.thrift THRIFT_CC_FILES_LIST THRIFT_GEN_INCLUDE_DIR)
include_directories(${THRIFT_GEN_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

if(CCSTORM_BUILD_TESTS)
    enable_testing()
endif()

file(GLOB_RECURSE SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")
file(GLOB_RECURSE TEST_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cc")

add_executable(hello ${THRIFT_CC_FILES_LIST} ${SOURCE})
target_link_libraries(hello ${LIBS})

SET(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "Headers install directory")
SET(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE STRING "Library install directory")
SET(DOC_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/doc/${PROJECT_NAME}" CACHE PATH "Document install directory")

IF(UNIX OR CYGWIN)
    SET(_CMAKE_INSTALL_DIR "${LIB_INSTALL_DIR}/cmake/${PROJECT_NAME}")
ELSEIF(WIN32)
    SET(_CMAKE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/cmake")
ENDIF()
SET(CMAKE_INSTALL_DIR "${_CMAKE_INSTALL_DIR}" CACHE PATH "Cmake files install directory")

# pkg-config
IF (UNIX OR CYGWIN)
  CONFIGURE_FILE (${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.pc.in
                  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
                  @ONLY)
  INSTALL (FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
      DESTINATION "${LIB_INSTALL_DIR}/pkgconfig"
      COMPONENT pkgconfig)
ENDIF()

install(DIRECTORY include/storm
    DESTINATION "${INCLUDE_INSTALL_DIR}"
    COMPONENT dev)

# ===============================
# Provide config and version files to be used by other applications
# ===============================
export(PACKAGE ${PROJECT_NAME})

# cmake-modules
CONFIGURE_FILE(${PROJECT_NAME}Config.cmake.in
    ${PROJECT_NAME}Config.cmake
    @ONLY)
CONFIGURE_FILE(${PROJECT_NAME}ConfigVersion.cmake.in
    ${PROJECT_NAME}ConfigVersion.cmake
    @ONLY)
INSTALL(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION "${CMAKE_INSTALL_DIR}"
    COMPONENT dev)
