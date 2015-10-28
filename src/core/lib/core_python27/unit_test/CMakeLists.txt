CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( python27_unit_test )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGPython27Project )
IF( NOT PYTHON_FOUND )
	RETURN()
ENDIF()


SET( ALL_SRCS
	python27_unit_test.cpp
	main.cpp
	pch.hpp
	pch.cpp
	test_fixture.hpp
	test_fixture.cpp
	../../core_generic_plugin_manager/default_context_manager.hpp
	../../core_generic_plugin_manager/default_context_manager.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )
BW_ADD_EXECUTABLE( ${PROJECT_NAME} ${BLOB_SRCS} )
 
BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_dependency_system
	core_reflection
	core_unit_test
	core_python27
	core_logging
)

#BW_PRECOMPILED_HEADER( ${PROJECT_NAME} pch.hpp )
BW_ADD_TOOL_TEST( core_common_unit_test )
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Unit Tests" )
