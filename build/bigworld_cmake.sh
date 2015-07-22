#!/bin/sh
BUILD_DIR="$(cd "$(dirname ${0})" && pwd -P )"
python ${BUILD_DIR}/bigworld_cmake.py
