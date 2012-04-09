cmake_minimum_required(VERSION 2.6)

project(atl)
set (SRC_LIST atom.c attr.c lookup3.c tclHash.c)

include(CheckFunctionExists)
include(CheckIncludeFiles)
include(CheckTypeSize)

CHECK_TYPE_SIZE(double SIZEOF_DOUBLE)
CHECK_TYPE_SIZE(float SIZEOF_FLOAT)
CHECK_TYPE_SIZE(int SIZEOF_INT)
CHECK_TYPE_SIZE(short SIZEOF_SHORT)

IF( NOT CMAKE_BUILD_TYPE )
SET( CMAKE_BUILD_TYPE "RelWithDebInfo" )
ENDIF()

find_package (Threads)

add_library(atl SHARED ${SRC_LIST})
add_library(atl-static STATIC ${SRC_LIST})

# The library target "atl" already has a default OUTPUT_NAME of "atl", so we don't need to change it.
# The library target "atl-static" has a default OUTPUT_NAME of "atl-static", so change it.
SET_TARGET_PROPERTIES(atl-static PROPERTIES OUTPUT_NAME "atl")
# Now the library target "foo-static" will be named "foo.lib" with MS tools.
# This conflicts with the "foo.lib" import library corresponding to "foo.dll",
# so we add a "lib" prefix (which is default on other platforms anyway):
SET_TARGET_PROPERTIES(atl-static PROPERTIES PREFIX "lib")

target_link_libraries (atl-static ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (atl ${CMAKE_THREAD_LIBS_INIT})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

CHECK_INCLUDE_FILES(malloc.h HAVE_MALLOC_H)
CHECK_INCLUDE_FILES(unistd.h HAVE_UNISTD_H)
CHECK_INCLUDE_FILES(stdlib.h HAVE_STDLIB_H)
CHECK_INCLUDE_FILES(string.h HAVE_STRING_H)
CHECK_INCLUDE_FILES(sys/time.h HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILES(windows.h HAVE_WINDOWS_H)

IF (NOT DEFINED (ATOM_SERVER_HOST) )
   set (ATOM_SERVER_HOST "atomhost.cercs.gatech.edu")
ENDIF()

CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/config.h.cmake ${CMAKE_SOURCE_DIR}/config.h )

INSTALL(FILES atl.h DESTINATION include)
INSTALL(TARGETS atl atl-static
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)


# display status message for important variables
MESSAGE( STATUS )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}" )
MESSAGE( STATUS "ATOM_SERVER_HOST = ${ATOM_SERVER_HOST}" )
MESSAGE( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS )

set (PROGS attr_test atom_test atom_server attr_dump) 
foreach (PROG ${PROGS})
    ADD_EXECUTABLE(${PROG} ${PROG}.c)
    TARGET_LINK_LIBRARIES(${PROG} atl)
endforeach()

ADD_EXECUTABLE(atom_check atom_check.c)
TARGET_LINK_LIBRARIES(atom_check atl )

ENABLE_TESTING()
ADD_TEST(atom_check atom_check)
