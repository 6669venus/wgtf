@echo off
rem Use the py launcher instead of directly executing python
rem The py launcher is installed into the windows path (C:Windows)
rem Can execute specific version using -2 or -3 argument ("py --help" for details)
@py %~dp0\qrc_generator.py %*
