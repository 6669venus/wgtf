CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( plg_python27 )

INCLUDE( BWMacros )
INCLUDE( WGPython27Project )
IF( NOT PYTHON_FOUND )
	RETURN()
ENDIF()

SET( ALL_SRCS
	pch.cpp
	pch.hpp
	plg_python27.cpp
	interpreter.cpp
	interpreter.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_TOOL_PLUGIN( plg_python27 ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${BLOB_SRCS}
)
 
BW_TARGET_LINK_LIBRARIES( plg_python27 PRIVATE
	core_generic_plugin
	libpython-shared
	wg_pyscript
)

BW_PRECOMPILED_HEADER( plg_python27 pch.hpp )
BW_PROJECT_CATEGORY( plg_python27 "Plugins" )

