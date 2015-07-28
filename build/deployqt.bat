SET PATH=%1;%PATH%
rem Strip off the first parameter to forward all parameters to windeployqt
shift
set params=%1
:loop
shift
if [%1]==[] goto afterloop
set params=%params% %1
goto loop
:afterloop
windeployqt.exe %params%