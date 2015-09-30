CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( plg_python27 )

INCLUDE( BWMacros )
INCLUDE( WGPython27Project )
IF( NOT PYTHON_FOUND )
	RETURN()
ENDIF()

SET( METADATA_SRCS
	metadata/scenario.mpp
)
SOURCE_GROUP( "MetaData" FILES ${METADATA_SRCS} )

SET( ALL_SRCS
	definer.cpp
	definer.hpp
	module.cpp
	module.hpp
	pch.cpp
	pch.hpp
	plg_python27.cpp
	reflection_module.cpp
	reflection_module.hpp
	scenario.cpp
	scenario.hpp
	scripting_engine.cpp
	scripting_engine.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
	${METADATA_SRCS}
)
BW_ADD_TOOL_PLUGIN( plg_python27 ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${BLOB_SRCS}
)
 
BW_TARGET_LINK_LIBRARIES( plg_python27 PRIVATE
	core_generic_plugin
	core_reflection
	libpython-shared
	wg_pyscript
)

BW_PRECOMPILED_HEADER( plg_python27 pch.hpp )
BW_PROJECT_CATEGORY( plg_python27 "Plugins" )

