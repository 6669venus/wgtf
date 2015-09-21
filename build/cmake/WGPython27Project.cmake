# INCUDE( WGPython27Project ) in projects that #include or link to Python 2.7.
FIND_PACKAGE( Python "2.7.10" )

IF( PYTHON_FOUND )
	INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_DIRS} )
ELSE()
	# Use ADD_CUSTOM_TARGET because this target has no cpp files
	ADD_CUSTOM_TARGET( ${PROJECT_NAME} COMMENT "${PROJECT_NAME} disabled." )
	BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Disabled" )
ENDIF()

