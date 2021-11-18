#!/bin/bash

BUILD_BIN="yaasc"
BUILD_DIR="build"

if [ -d "${BUILD_DIR}" ]; then
  make clean
fi

make

if [ -e "${BUILD_DIR}/${BUILD_BIN}" ]; then
  echo
  ./${BUILD_DIR}/${BUILD_BIN}
fi

echo