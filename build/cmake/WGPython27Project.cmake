# INCUDE( WGPython27Project ) in projects that #include or link to Python 2.7.
FIND_PACKAGE( Python ${PYTHON_VERSION} REQUIRED )

IF( PYTHON_FOUND )
	INCLUDE_DIRECTORIES( ${PYTHON_DIR}/Include )
	IF( WIN32 )
		INCLUDE_DIRECTORIES( ${PYTHON_DIR}/PC )
	ENDIF()
ENDIF()


