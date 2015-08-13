cd ..
git clean -dxf bin

REM Hack for script not checking for empty win32 unit tests.
mkdir bin\unit_tests\win32
echo > bin\unit_tests\win32\empty.txt

cd build\bw_internal\scripts

C:\Python27\python cmake_all_windows.py --type continuous --visual_studio 2012 --devEnvCom "C:\Program Files (x86)\Microsoft Visual Studio 11.0\Common7\IDE\devenv.com" --include-proj generic_app_test

cd ..\..
