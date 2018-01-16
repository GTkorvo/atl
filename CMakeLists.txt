cmake_minimum_required(VERSION 2.8.3)
cmake_policy(VERSION 2.8.3)
project(atl)

 
include(CheckFunctionExists)
include(CheckIncludeFiles)
include(CheckTypeSize)
include(CTest)
mark_as_advanced(BUILD_TESTING)

set(ATL_MAJOR_VERSION 2)
set(ATL_MINOR_VERSION 2)
set(ATL_PATCH_VERSION 0)
set(ATL_VERSION
  ${ATL_MAJOR_VERSION}.${ATL_MINOR_VERSION}.${ATL_PATCH_VERSION})

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/CMake/atl)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")
 
# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()
 
# set up include-directories
include_directories(
  "${PROJECT_SOURCE_DIR}"   # to find foo/foo.h
  "${PROJECT_BINARY_DIR}")  # to find foo/config.h
 
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set (SRC_LIST atom.c attr.c lookup3.c tclHash.c)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)


# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")

set (CMAKE_MACOSX_RPATH 1)

CHECK_TYPE_SIZE(double SIZEOF_DOUBLE)
CHECK_TYPE_SIZE(float SIZEOF_FLOAT)
CHECK_TYPE_SIZE(int SIZEOF_INT)
CHECK_TYPE_SIZE(short SIZEOF_SHORT)

IF( NOT CMAKE_BUILD_TYPE )
SET( CMAKE_BUILD_TYPE "RelWithDebInfo" )
ENDIF()

if(NOT ANDROID)
  find_package (Threads)
endif (NOT ANDROID)

set (LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})

foreach(arg ${DEPLIBS})
   set(DEP_STRING "${DEP_STRING} ${arg}")
endforeach(arg ${DEPLIBS})

list (APPEND LINK_LIBRARIES ${CMAKE_THREADS_LIBS_INIT})

CHECK_INCLUDE_FILES(malloc.h HAVE_MALLOC_H)
CHECK_INCLUDE_FILES(unistd.h HAVE_UNISTD_H)
CHECK_INCLUDE_FILES(stdlib.h HAVE_STDLIB_H)
CHECK_INCLUDE_FILES(string.h HAVE_STRING_H)
CHECK_INCLUDE_FILES(sys/time.h HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILES(windows.h HAVE_WINDOWS_H)

IF (NOT DEFINED (ATOM_SERVER_HOST) )
   set (ATOM_SERVER_HOST "atomhost.cercs.gatech.edu")
ENDIF()

CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )

add_library(atl ${SRC_LIST})

set(ATL_VERSION_MAJOR 2)
set(ATL_VERSION_MINOR 1)
set(ATL_VERSION_PATCH 3)
set(ATL_VERSION_STRING ${ATL_VERSION_MAJOR}.${ATL_VERSION_MINOR}.${ATL_VERSION_PATCH})
set_target_properties(atl PROPERTIES VERSION ${ATL_VERSION_STRING}
                                          SOVERSION ${ATL_VERSION_MAJOR})
set_target_properties(atl PROPERTIES PUBLIC_HEADER "atl.h")
 
INSTALL(FILES atl.h DESTINATION include)
install(TARGETS atl
  # IMPORTANT: Add the foo library to the "export-set"
  EXPORT atlTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/atl"
    COMPONENT dev)

target_include_directories(atl PUBLIC
                       $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		       $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
                       $<INSTALL_INTERFACE:${include_dest}>
                       $<INSTALL_INTERFACE:${lib_dest}>)


# display status message for important variables
MESSAGE( STATUS )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}" )
MESSAGE( STATUS "ATOM_SERVER_HOST = ${ATOM_SERVER_HOST}" )
MESSAGE( STATUS "LIBRARIES_ONLY = ${LIBRARIES_ONLY}" )
MESSAGE( STATUS "BUILD_TESTING = ${BUILD_TESTING}" )
MESSAGE( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS )

if (NOT LIBRARIES_ONLY)
    set (PROGS atom_server attr_dump)
endif()

set (INST_PROGS ${PROGS})

if (BUILD_TESTING)
  list (APPEND PROGS attr_test atom_test atom_check )
endif()

foreach (PROG ${PROGS})
    ADD_EXECUTABLE(${PROG} ${PROG}.c)
    TARGET_LINK_LIBRARIES(${PROG} atl)
endforeach()

SET(PROGS_INSTALL)
# prefix and suffix elements
foreach(l ${INST_PROGS})
   list(APPEND PROGS_INSTALL ${CMAKE_CURRENT_BINARY_DIR}/${l} )
endforeach()

INSTALL(PROGRAMS ${PROGS_INSTALL} DESTINATION bin)

 
# Add all targets to the build-tree export set
export(TARGETS atl
  FILE "${PROJECT_BINARY_DIR}/atlTargets.cmake")
 
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE atl)
 
# Create the atlConfig.cmake and atlConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
   "${INSTALL_INCLUDE_DIR}")
# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(atlConfig.cmake.in
  "${PROJECT_BINARY_DIR}/atlConfig.cmake" @ONLY)
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${ATL_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(atlConfig.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/atlConfig.cmake" @ONLY)
# ... for both
configure_file(atlConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/atlConfigVersion.cmake" @ONLY)
 
# Install the atlConfig.cmake and atlConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/atlConfig.cmake"
  "${PROJECT_BINARY_DIR}/atlConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)
 
# Install the export set for use with the install-tree
install(EXPORT atlTargets DESTINATION
  "${INSTALL_CMAKE_DIR}" COMPONENT dev)

if (BUILD_TESTING)
ENABLE_TESTING()
ADD_TEST(atom_check atom_check)
endif ()

set (CTEST_MEMORYCHECK_SUPPRESSIONS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/atl.supp)


