# - Try to find the TINYXML2 library
# Once done this will define
#
#  TINYXML2_FOUND - system has TINYXML2
#  TINYXML2_INCLUDE_DIR - the TINYXML2 include directory
#  TINYXML2_SOURCES - the TINYXML2 source files

set(FIND_TINYXML2_PATHS C:/local/tinyxml2-master) # On Windows, this is where my tinyxml2 folder is

find_path(TINYXML2_INCLUDE_DIR tinyxml2.h tinyxml2.cpp  # The variable to store the path in and the name of the header files
        PATH_SUFFIXES include               # The folder name containing the header files
        PATHS ${FIND_TINYXML2_PATHS}
        /usr/include
        /usr/local/include)

find_library(TINYXML2_LIBRARY               # The variable to store where it found the .a files
        NAMES tinyxml2                      # The name of the .a file (without the extension and without the 'lib')
        PATH_SUFFIXES lib                   # The folder the .a file is in
PATHS ${FIND_TINYXML2_PATHS})               # Where to look (defined above)

SET(TINYXML2_FOUND "NO")
IF (TINYXML2_INCLUDE_DIR)
	SET(TINYXML2_FOUND "YES")
ENDIF (TINYXML2_INCLUDE_DIR)

set(TINYXML2_SOURCES ${TINYXML2_INCLUDE_DIR}/tinyxml2.cpp)

if(TINYXML2_INCLUDE_DIR)
   message(STATUS "Found TINYXML2: ${TINYXML2_INCLUDE_DIR}")
else(TINYXML2_INCLUDE_DIR)
  if (NOT TINYXML2_FIND_QUIETLY)
    message(FATAL_ERROR "could NOT find TINYXML2")
  endif (NOT TINYXML2_FIND_QUIETLY)
endif(TINYXML2_INCLUDE_DIR)

MARK_AS_ADVANCED(TINYXML2_INCLUDE_DIR TINYXML2_LIBRARY TINYXML2_SOURCES)