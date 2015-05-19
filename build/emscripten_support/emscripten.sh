#!/bin/sh
#
# Helper script to set various paths for running emscripten
#
# Usage: emscripten.sh ${BW_ABS_SRC} ${BW_THIRD_PARTY_INSTALL_DIR} ${EMSCRIPTEN_COMMAND}
#
# BW_ABS_SRC						The absolute path to the directory containing
#								bigworld source.
# BW_THIRD_PARTY_INSTALL_DIR	The absolute path to the directory containing
#								bigworld installed third party binaries.
# EMSCRIPTEN_COMMAND			The emscripten command to run.
#								e.g. (emcc, emc++, etc.)
#

BW_ABS_SRC=$1
BW_THIRD_PARTY_INSTALL_DIR=$2
EMSCRIPTEN_COMMAND=$3

shift 3

# We rely on the Python 2.7 binary to have been installed as part of building
# ProcessDefs before we run emscripten. We can't use the system python on
# CentOS 5 because we require at least Python 2.7.
export PYTHONHOME=${BW_THIRD_PARTY_INSTALL_DIR}/python
PYTHON=${PYTHONHOME}/bin/python.exe

test -s $PYTHON || echo "Error: $PYTHON does not exist! Please install python by running: make python_install in programming/bigworld "

# Set up variables to be inherited by the command:
export EMSCRIPTEN="${BW_ABS_SRC}/third_party/emscripten"
export EM_CONFIG="${BW_ABS_SRC}/build/emscripten_support/emscripten.config"
export EM_CACHE="${BW_ABS_SRC}/build/emscripten_cache"
export PYTHON="${PYTHON}"
exec ${PYTHON} ${EMSCRIPTEN}/${EMSCRIPTEN_COMMAND} "$@"
