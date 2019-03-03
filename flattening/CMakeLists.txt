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

## CGAL dependencies on Windows: GMP & MPFR
# Shortcut function
function(download_project name)
  download_project(
    PROJ         ${name}
    SOURCE_DIR   ${PROJECT_SOURCE_DIR}/${name}
    DOWNLOAD_DIR ${PROJECT_SOURCE_DIR}/.cache/${name}
    ${ARGN}
  )
endfunction()


function(download_cgal_deps)
  if(WIN32)
    download_project(gmp
        URL     https://cgal.geometryfactory.com/CGAL/precompiled_libs/auxiliary/x64/GMP/5.0.1/gmp-all-CGAL-3.9.zip
        URL_MD5 508c1292319c832609329116a8234c9f
    )
    download_project(mpfr
        URL https://cgal.geometryfactory.com/CGAL/precompiled_libs/auxiliary/x64/MPFR/3.0.0/mpfr-all-CGAL-3.9.zip
        URL_MD5 48840454eef0ff18730050c05028734b
    )
    set(ENV{GMP_DIR} "${PROJECT_SOURCE_DIR}/gmp")
    set(ENV{MPFR_DIR} "${PROJECT_SOURCE_DIR}/mpfr")

  # Note: It IS important to download different files on different OS's:
  # on Unix-like systems, we need the file persmissions (only available in the .tar.gz),
  # while on Windows, we need CR/LF line feeds (only available in the .zip)

  # Boost 1.65 is problematic because of https://github.com/boostorg/build/issues/230
  # Boost 1.66 is problematic because of https://github.com/boostorg/build/issues/273
  if(UNIX)
    set(BOOST_URL "https://dl.bintray.com/boostorg/release/1.65.1/source/boost_1_65_1.tar.gz")
    set(BOOST_HASH SHA256=a13de2c8fbad635e6ba9c8f8714a0e6b4264b60a29b964b940a22554705b6b60)
    set(BOOST_BOOTSTRAP_COMMAND ./bootstrap.sh)
    set(BOOST_B2_COMMAND ./b2)
  elseif(WIN32)
    set(BOOST_URL "https://dl.bintray.com/boostorg/release/1.65.1/source/boost_1_65_1.zip")
    set(BOOST_HASH SHA256=d1775aef807bd6a14077b450cce2950d8eacd86aaf51015f79e712917f8cc3c2)
    set(BOOST_BOOTSTRAP_COMMAND cmd /C bootstrap.bat msvc)
    set(BOOST_B2_COMMAND b2.exe)
  endif()

  # We only support x64 builds
  set(BOOST_ADDRESS_MODEL 64)

  if(MSVC_VERSION EQUAL 1500) #VS2008
    set(BOOST_TOOLSET "--toolset=msvc-9.0")
  elseif(MSVC_VERSION EQUAL 1600) #VS2010
    set(BOOST_TOOLSET "--toolset=msvc-10.0")
  elseif(MSVC_VERSION EQUAL 1700) #VS2012
    set(BOOST_TOOLSET "--toolset=msvc-11.0")
  elseif(MSVC_VERSION EQUAL 1800) #VS2013
    set(BOOST_TOOLSET "--toolset=msvc-12.0")
  elseif(MSVC_VERSION EQUAL 1900) #VS2015
    set(BOOST_TOOLSET "--toolset=msvc-14.0")
  elseif(NOT (MSVC_VERSION LESS 1910)) #VS2017
    set(BOOST_TOOLSET "--toolset=msvc-14.1")
  endif()

  # Boost
  set(BOOST_DIR "${CMAKE_CURRENT_LIST_DIR}/boost")
  ExternalProject_Add(Boost_Project
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND ""

    PREFIX "${BOOST_DIR}"
    BUILD_IN_SOURCE 1

    URL ${BOOST_URL}
    URL_HASH ${BOOST_HASH}

    UPDATE_COMMAND ""
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_LIST_DIR}/os.jam" "tools/build/src/util/os.jam"
    CONFIGURE_COMMAND ${BOOST_BOOTSTRAP_COMMAND} --prefix=${BOOST_DIR}/lib
    BUILD_COMMAND ${BOOST_B2_COMMAND} install -j8 -d0 --prefix=${BOOST_DIR} --with-filesystem --with-system --with-thread --with-serialization --with-date_time address-model=${BOOST_ADDRESS_MODEL} link=static runtime-link=static ${BOOST_TOOLSET}
    INSTALL_COMMAND ""
  )
  endif()
endfunction()

# CGAL
#set(CGAL_DIR "${PROJECT_SOURCE_DIR}/cgal")
#download_cgal_deps()
#set(CGAL_Boost_USE_STATIC_LIBS ON CACHE BOOL "" FORCE)
#find_package(CGAL CONFIG COMPONENTS Core PATHS ${CGAL_DIR} NO_DEFAULT_PATH)

find_package(CGAL QUIET COMPONENTS Core)
include(${CGAL_USE_FILE})

# tinyxml2
set(TINYXML2_DIR "${PROJECT_SOURCE_DIR}/tinyxml2/")
add_library(tinyxml2 STATIC ${TINYXML2_DIR}/tinyxml2.cpp ${TINYXML2_DIR}/tinyxml2.h)
set_target_properties(tinyxml2 PROPERTIES
        COMPILE_DEFINITIONS "TINYXML2_EXPORT"
        VERSION "3.0.0"
        SOVERSION "3")
set(module_libname "xml")
add_library(${module_libname} INTERFACE)
target_link_libraries(xml INTERFACE tinyxml2)
target_include_directories(xml INTERFACE ${TINYXML2_DIR})
INCLUDE_DIRECTORIES( "${TINYXML2_DIR}" )

# nanosvg
set(NANOSVG_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/nanosvg/src")
#add_library(nanosvg STATIC ${NANOSVG_INCLUDE_DIR}/src/nanosvg.h ${NANOSVG_INCLUDE_DIR/src/nanosvgrast.h)
INCLUDE_DIRECTORIES ( "${NANOSVG_INCLUDE_DIR}" )

# Eigen 3.2.10
set(EIGEN3_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/eigen/")
INCLUDE_DIRECTORIES ( "${EIGEN3_INCLUDE_DIR}" )

# mine
include_directories(BEFORE "visible_strokes/include")

file(GLOB SRCFILES visible_strokes/src/*.cpp)
list(APPEND SRCFILES ${TINYXML2_DIR}/tinyxml2.cpp)
list(APPEND SRCFILES visible_strokes/main.cpp)

add_executable(flattening ${SRCFILES})
target_link_libraries(flattening ${CGAL_LIBRARIES})
target_link_libraries(flattening ${xml})

