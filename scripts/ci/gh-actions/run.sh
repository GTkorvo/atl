#!/bin/bash

readlinkf() { perl -MCwd -e 'print Cwd::abs_path shift' "$1"; }

set -x
set -e
env | sort

if [ "$1" = "setup" ]
then
  if [ -e scripts/ci/setup/${GH_YML_JOBNAME}.sh ]
  then
    source scripts/ci/setup/${GH_YML_JOBNAME}.sh
  fi
  exit 0
fi

if [ -z "${CTEST}" ]
then
  CTEST=ctest
fi

SOURCE_DIR=$(readlinkf ${GITHUB_WORKSPACE})
BUILD_DIR=$(readlinkf ${GITHUB_WORKSPACE}/..)/build

CTEST_ARGS="-DCTEST_BUILD_WARNINGS_AS_ERRORS=ON -DCTEST_SITE=GitHub-Actions -DCTEST_BUILD_NAME=${GH_YML_JOBNAME} -DCTEST_SOURCE_DIRECTORY=${SOURCE_DIR} -DCTEST_BINARY_DIRECTORY=${BUILD_DIR} -Ddashboard_do_submit=OFF -Ddashboard_full=OFF"
case $1 in
  configure)
    CTEST_ARGS="${CTEST_ARGS} -Ddashboard_fresh=ON -Ddashboard_do_configure=ON"
    ;;
  build)
    CTEST_ARGS="${CTEST_ARGS} -Ddashboard_do_build=ON"
    ;;
  test)
    CTEST_ARGS="${CTEST_ARGS} -Ddashboard_do_test=ON"
    ;;
esac

shift
${CTEST} -VV -S ${SOURCE_DIR}/scripts/dashboard/atl_common.cmake ${CTEST_ARGS} "$@"
