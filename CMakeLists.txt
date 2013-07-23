cmake_minimum_required(VERSION 2.8.3)
cmake_policy(VERSION 2.8.3)
project(atl)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set (SRC_LIST atom.c attr.c lookup3.c tclHash.c)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(CheckFunctionExists)
include(CheckIncludeFiles)
include(CheckTypeSize)
include(CreateLibtoolFile)
include(FindCERCSProject)
include(AddLibtoolLibrary)
include(CTest)

IF(NOT DEFINED BUILD_SHARED_STATIC)
  SET( BUILD_SHARED_STATIC "BOTH")
ENDIF(NOT DEFINED BUILD_SHARED_STATIC)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

CHECK_TYPE_SIZE(double SIZEOF_DOUBLE)
CHECK_TYPE_SIZE(float SIZEOF_FLOAT)
CHECK_TYPE_SIZE(int SIZEOF_INT)
CHECK_TYPE_SIZE(short SIZEOF_SHORT)

IF( NOT CMAKE_BUILD_TYPE )
SET( CMAKE_BUILD_TYPE "RelWithDebInfo" )
ENDIF()

if (DEFINED CERCS_USE_INSTALLED) 
   set (CERCS_USE_INSTALLED "USE_INSTALLED")
endif(DEFINED CERCS_USE_INSTALLED) 

if(NOT ANDROID)
  find_package (Threads)
endif (NOT ANDROID)

find_CERCS_project(cercs_env LIBRARY cercs_env INCLUDES cercs_env.h REQUIRED ${CERCS_USE_INSTALLED})

set (LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})

if (${CERCS_ENV_FOUND})
   list (APPEND LINK_LIBRARIES -lcercs_env)
   LINK_DIRECTORIES(${CERCS_ENV_LIB_DIR})
   INCLUDE_DIRECTORIES(${CERCS_ENV_INCLUDE_DIR})
   list (APPEND DEPLIBS "-L${CERCS_ENV_LIB_DIR}" "${CERCS_ENV_LIB_DIR}/libcercs_env.la")
endif()

foreach(arg ${DEPLIBS})
   set(DEP_STRING "${DEP_STRING} ${arg}")
endforeach(arg ${DEPLIBS})

list (APPEND LINK_LIBRARIES ${CMAKE_THREADS_LIBS_INIT})

add_libtool_library(NAME atl SRC_LIST ${SRC_LIST} DEP_LIBS "${DEP_STRING}" LINK_LIBS ${LINK_LIBRARIES})

CHECK_INCLUDE_FILES(malloc.h HAVE_MALLOC_H)
CHECK_INCLUDE_FILES(unistd.h HAVE_UNISTD_H)
CHECK_INCLUDE_FILES(stdlib.h HAVE_STDLIB_H)
CHECK_INCLUDE_FILES(string.h HAVE_STRING_H)
CHECK_INCLUDE_FILES(sys/time.h HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILES(windows.h HAVE_WINDOWS_H)

IF (NOT DEFINED (ATOM_SERVER_HOST) )
   set (ATOM_SERVER_HOST "atomhost.cercs.gatech.edu")
ENDIF()

if (${CMAKE_C_COMPILER_ID} MATCHES "Intel") 
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -shared-intel")
endif()

CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )

INSTALL(FILES atl.h DESTINATION include)
INSTALL(TARGETS ${INSTALL_TARGETS}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)


# display status message for important variables
MESSAGE( STATUS )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}" )
MESSAGE( STATUS "BUILD_SHARED_STATIC = ${BUILD_SHARED_STATIC}")
MESSAGE( STATUS "ATOM_SERVER_HOST = ${ATOM_SERVER_HOST}" )
MESSAGE( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS )

set (PROGS attr_test atom_test atom_check atom_server attr_dump) 
foreach (PROG ${PROGS})
    ADD_EXECUTABLE(${PROG} ${PROG}.c)
    TARGET_LINK_LIBRARIES(${PROG} atl)
endforeach()

SET(PROGS_INSTALL)
# prefix and suffix elements
foreach(l ${PROGS})
   list(APPEND PROGS_INSTALL ${CMAKE_CURRENT_BINARY_DIR}/${l} )
endforeach()

INSTALL(PROGRAMS ${PROGS_INSTALL} DESTINATION bin)

#ADD_EXECUTABLE(atom_check atom_check.c)
TARGET_LINK_LIBRARIES(atom_check atl )

ENABLE_TESTING()
set (CTEST_MEMORYCHECK_SUPPRESSIONS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/atl.supp)
ADD_TEST(atom_check atom_check)
