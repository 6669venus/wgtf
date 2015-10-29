CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( core_python27 )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )
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
	type_converters/i_type_converter.hpp
	type_converters/long_converter.cpp
	type_converters/long_converter.hpp
	type_converters/type_converter.cpp
	type_converters/type_converter.hpp
	type_converters/string_converter.cpp
	type_converters/string_converter.hpp
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
BW_ADD_LIBRARY( ${PROJECT_NAME} ${BLOB_SRCS} )
 
BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_dependency_system
	core_python_script
	core_reflection
	libpython-shared
	wg_pyscript
)

BW_PRECOMPILED_HEADER( ${PROJECT_NAME} pch.hpp )
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "NGT Libs" )
