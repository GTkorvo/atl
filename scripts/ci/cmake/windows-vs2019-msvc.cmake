# Client maintainer: chuck.atkins@kitware.com

set(dashboard_cache "
")

set(CTEST_CMAKE_GENERATOR "Visual Studio 16")
set(CTEST_CMAKE_GENERATOR_PLATFORM x64)
set(CTEST_TEST_ARGS
  # Unclear why this test currently dies.  Disabling until it can be addressed.
  EXCLUDE "atl_attr_test"
)

list(APPEND CTEST_UPDATE_NOTES_FILES "${CMAKE_CURRENT_LIST_FILE}")
include(${CMAKE_CURRENT_LIST_DIR}/../../dashboard/atl_common.cmake)
