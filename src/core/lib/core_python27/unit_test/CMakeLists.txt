CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( python27_unit_test )

INCLUDE( BWMacros )
INCLUDE( WGPython27Project )

IF( NOT PYTHON_FOUND )
	RETURN()
ENDIF()

SET( ALL_SRCS
	python27_unit_test.cpp
	main.cpp
	memory_plugin_context_creator.cpp
	memory_plugin_context_creator.hpp
	pch.hpp
	pch.cpp
	plugin_memory_allocator.cpp
	plugin_memory_allocator.hpp
	reflection_test_module.cpp
	reflection_test_module.hpp
	test_plugins_fixture.hpp
	test_plugins_fixture.cpp
	test_python_fixture.hpp
	test_python_fixture.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )
BW_ADD_EXECUTABLE( ${PROJECT_NAME} ${BLOB_SRCS} )
 
BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_generic_plugin_manager
	wg_memory
	core_dependency_system
	core_reflection
	core_unit_test
	core_python27
	core_logging
)

BW_PRECOMPILED_HEADER( ${PROJECT_NAME} pch.hpp )
BW_ADD_TOOL_TEST( ${PROJECT_NAME} )
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Unit Tests" )

IF( BW_PYTHON_DLL_SUPPORT )
	BW_COPY_TARGET( ${PROJECT_NAME} libpython-shared )
	BW_COPY_TARGET_PDB( ${PROJECT_NAME} libpython-shared )
ENDIF()
