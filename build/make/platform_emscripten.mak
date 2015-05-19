#
# This file contains platform-specific options for emscripten.
#

EMSCRIPTEN_SH := $(BW_ABS_SRC)/build/emscripten_support/emscripten.sh

CXX11_CXXFLAGS := -std=c++11

CC 		:= ${EMSCRIPTEN_SH} ${BW_ABS_SRC} ${BW_INSTALL_DIR}/${BW_THIRD_PARTY_INSTALL_DIR} emcc
CXX 	:= ${EMSCRIPTEN_SH} ${BW_ABS_SRC} ${BW_INSTALL_DIR}/${BW_THIRD_PARTY_INSTALL_DIR} em++
AR		:= ${EMSCRIPTEN_SH} ${BW_ABS_SRC} ${BW_INSTALL_DIR}/${BW_THIRD_PARTY_INSTALL_DIR} emar
RANLIB 	:= ${EMSCRIPTEN_SH} ${BW_ABS_SRC} ${BW_INSTALL_DIR}/${BW_THIRD_PARTY_INSTALL_DIR} emranlib

# Disable some warnings
WARNING_FLAGS = -Wno-warn-absolute-paths -Wno-unused-function

CPPFLAGS	:= $(filter-out -O3, $(CPPFLAGS))
CPPFLAGS 	+= -O2
CXXFLAGS 	+= $(WARNING_FLAGS) -DMF_SINGLE_THREADED $(CXX11_CXXFLAGS)
CFLAGS 		+= $(WARNING_FLAGS)
ARCHFLAGS   += 


OPENSSL_CONFIG_TARGET  := cc
OPENSSL_CONFIG_OPTIONS := no-threads
#OPENSSL_CONFIG_FLAGS   := -g
OPENSSL_CONFIG_FLAGS   += -fPIC -DOPENSSL_PIC

BW_PLATFORM_PRE_BUILD_TARGETS += host_python_install

# vi: filetype=make
# platform_emscripten.mak - build
