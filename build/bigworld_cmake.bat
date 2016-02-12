@echo off
where py >nul 2>nul
if %ERRORLEVEL% equ 0 (
	@py -2 bigworld_cmake.py
) else (
	@python bigworld_cmake.py
)
@pause