#!/bin/sh
sh ..\bigworld_cmake.sh --target "generic_app_test_deploy" --generator "xcode" --qt-version "5.5.1" 

cd ../../build_generic_app_test_deploy_xcode_qt5.5.1
./rebuild_hybrid.sh
./rebuild_debug.sh

