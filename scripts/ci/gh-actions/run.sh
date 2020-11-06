#!/bin/bash

if [ "$1" = "setup" ]
then
  if [ -e scripts/ci/setup/${GH_YML_JOBNAME}.sh ]
  then
    source scripts/ci/setup/${GH_YML_JOBNAME}.sh
    exit $?
  fi
  exit 0
fi

if [ -z "${CTEST}" ]
then
  CTEST=ctest
fi

SOURCE_DIR=$(readlink -f ${GITHUB_WORKDIR})
BUILD_DIR=$(readlink -f ${GITHUB_WORKDIR}/..)/build

CTEST_ARGS="-DCTEST_SOURCE_DIRECTORY=${SORUCE_DIR} -DCTEST_BINARY_DIRECTORY=${BUILD_DIR} -Ddashboard_do_submit=OFF -Ddashboard_do_full=OFF"
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

${CTEST} -VV -D ${SOURCE_DIR}/scripts/dashboard/atl.cmake ${CTEST_ARGS}
