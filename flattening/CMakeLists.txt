cmake_minimum_required(VERSION 3.9.4)
project(flattening)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /bigobj")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR} )
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR} )
    set(CMAKE_GENERATOR_PLATFORM "Visual Studio 15 2017 Win64")
    set(CMAKE_GENERATOR_TOOLSET "host=x64")
    set(CMAKE_VS_PLATFORM_NAME "x64")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "." )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
endif()

# CGAL
find_package(CGAL QUIET COMPONENTS Core)
#set(CGAL_DONT_OVERRIDE_CMAKE_FLAGS TRUE CACHE BOOL "CGAL's CMAKE Setup is super annoying ")
include(${CGAL_USE_FILE})

# tinyXML2
#find_package(TinyXML2 REQUIRED)
#include_directories(${TINYXML2_INCLUDE_DIR})

# Eigen
if(MSVC)
	if(EXISTS "$ENV{EIGEN3_INCLUDE_DIR}")
		set( EIGEN3_INCLUDE_DIR "$ENV{EIGEN3_INCLUDE_DIR}" )
	endif()
	if( NOT EIGEN3_INCLUDE_DIR )
    	MESSAGE( FATAL_ERROR "Please point the environment variable EIGEN3_DIR to the include directory of your Eigen3 installation.")
	endif()
else()
	find_path(EIGEN3_INCLUDE_DIR
        /usr/include
        /usr/local/include
        PATH_SUFFIXES eigen3 eigen include)
    if( NOT EIGEN3_INCLUDE_DIR )
    	MESSAGE( FATAL_ERROR "Eigen3 isntallation was not found.")
	endif()
endif()

INCLUDE_DIRECTORIES ( "${EIGEN3_INCLUDE_DIR}" )


include_directories(BEFORE "visible_strokes/include")

file(GLOB SRCFILES visible_strokes/src/*.cpp)
list(APPEND SRCFILES visible_strokes/include/tinyxml2.cpp)
list(APPEND SRCFILES visible_strokes/main.cpp)

add_executable(flattening ${SRCFILES})
target_link_libraries(flattening ${CGAL_LIBRARIES})

