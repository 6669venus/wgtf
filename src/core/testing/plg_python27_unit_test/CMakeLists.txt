CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( plg_python27_unit_test )

INCLUDE( BWMacros )
INCLUDE( WGPython27Project )

IF( NOT PYTHON_FOUND )
	RETURN()
ENDIF()

SET( ALL_SRCS
	pch.hpp
	pch.cpp
	plg_python27_unit_test.cpp
	python27_unit_test.cpp
	python27_unit_test.hpp
	reflection_test_module.cpp
	reflection_test_module.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )
BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${BLOB_SRCS}
)
 
BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_generic_plugin
	core_reflection
	core_unit_test
	core_python27
	libpython-shared
	wg_pyscript
)

BW_PRECOMPILED_HEADER( ${PROJECT_NAME} pch.hpp )
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
