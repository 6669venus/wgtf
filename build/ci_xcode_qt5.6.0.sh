#!/bin/sh
cd ngt/build
sh bigworld_cmake.sh --target "generic_app_test_deploy" --generator "XCode" --qt-version "5.5.1" 

cd ../build_generic_app_test_deploy_xcode_qt5.6.0
./rebuild_hybrid.sh
./rebuild_debug.sh

