BUILD_DIR="$(cd "$(dirname ${0})" && pwd -P )"
cmake $BUILD_DIR/../src -Wno-dev -G"Xcode" -DBW_CMAKE_TARGET=generic_app_test -DQT_VERSION=5.5.0 -DBW_VERIFY_LINUX=OFF
