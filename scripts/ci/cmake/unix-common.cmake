# Client maintainer: chuck.atkins@kitware.com

if(NOT CTEST_CMAKE_GENERATOR)
  set(CTEST_CMAKE_GENERATOR "Unix Makefiles")
endif()
if(NOT CTEST_BUILD_FLAGS)
  set(CTEST_BUILD_FLAGS "-k -j2")
endif()

list(APPEND CTEST_UPDATE_NOTES_FILES "${CMAKE_CURRENT_LIST_FILE}")
include(${CMAKE_CURRENT_LIST_DIR}/../../dashboard/atl_common.cmake)
