cd ..\..

REM clean up output folder
rmdir /q /s out
git clean -dxf bin

REM Hack for script not checking for empty win32 unit tests.
mkdir bin\unit_tests\win32
echo > bin\unit_tests\win32\empty.txt

python.exe build\bw_internal\scripts\cmake_all_windows.py --type continuous --visual_studio 2013 --devEnvCom "C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\IDE\devenv.com" --include-proj generic_app_test_deploy --qt-version "5.6.0"

