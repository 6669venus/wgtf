@rem Delete history from previous run
del ..\bin\generic_app_test\win64\generic_app_test_cmd_history_ui_main_ver_*.txt
del ..\bin\generic_app_test\win64\generic_app_test_ui_main_ver_*.txt

@rem The version of Python in your PYTHONPATH should be 2.7
python.exe .\bw_internal\scripts\testing\runTests.py --url %BUILD_URL% --no-tools --no-batch-compiler -e generic_app_test
