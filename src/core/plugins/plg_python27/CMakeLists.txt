CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( plg_python27 )

INCLUDE( BWMacros )
INCLUDE( WGPython27Project )
IF( NOT PYTHON_FOUND )
	RETURN()
ENDIF()

SET( METADATA_SRCS
	metadata/defined_instance.mpp
	metadata/scenario.mpp
)
SOURCE_GROUP( "MetaData" FILES ${METADATA_SRCS} )

SET( TYPE_CONVERTER_SRCS
	type_converters/string_type_converter.cpp
	type_converters/string_type_converter.hpp
)
SOURCE_GROUP( "Type Converters" FILES ${TYPE_CONVERTER_SRCS} )

SET( ALL_SRCS
	defined_instance.cpp
	defined_instance.hpp
	definition_details.cpp
	definition_details.hpp
	module.cpp
	module.hpp
	pch.cpp
	pch.hpp
	plg_python27.cpp
	property.cpp
	property.hpp
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
	${TYPE_CONVERTER_SRCS}
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

