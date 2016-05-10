@ECHO OFF

COPY %~dp0\..\..\doc\core\Doxyfile.in  %~dp0\..\..\doc\core\Doxyfile

for %%a in (%~dp0\..\..\doc\core\Doxyfile) do (
find "@WG_TOOLS_SOURCE_DIR@" <"%%a" >nul && (
    type "%%a"| %~dp0\repl "@WG_TOOLS_SOURCE_DIR@" "%~dp0\..\..\src" >"%%a.tmp"
    move /y "%%a.tmp" "%%a" >nul
    )

find "@DOXYQML_EXECUTABLE@"  <"%%a" >nul && (
    type "%%a"| %~dp0\repl "@DOXYQML_EXECUTABLE@" "%~dp0\..\..\src\core\third_party\doxyqml\bin\doxyqml.bat" >"%%a.tmp"
    move /y "%%a.tmp" "%%a" >nul
    )

find "@GRAPHVIZ_DOT_PATH@" <"%%a" >nul && (
    type "%%a"| %~dp0\repl  "@GRAPHVIZ_DOT_PATH@" "%~dp0\..\..\src\core\third_party\Graphviz2.38\bin\dot.exe" >"%%a.tmp"
    move /y "%%a.tmp" "%%a" >nul
    )
)

%~dp0\..\..\src\core\third_party\doxygen\bin\doxygen.exe  %~dp0\..\..\doc\core\Doxyfile