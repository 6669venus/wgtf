CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( generic_plugin_system_unit_test )

INCLUDE( BWStandardProject )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/ngt/lib )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/ngt/tools )

SET( BW_USE_CSTDMF_DLL ON )
SET( BW_USE_CSTDMF_IN_DLL ON )
ADD_DEFINITIONS( 
	-DCSTDMF_IMPORT
)

SET( ALL_SRCS
	main.cpp
	memory_plugin_context_creator.cpp
	memory_plugin_context_creator.hpp
	pch.hpp
	pch.cpp
	plugin_memory_allocator.cpp
	plugin_memory_allocator.hpp
	test_plugin_system.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )
BW_ADD_EXECUTABLE( generic_plugin_system_unit_test ${BLOB_SRCS} )
BW_TARGET_LINK_LIBRARIES( generic_plugin_system_unit_test PRIVATE
	generic_plugin_system
	wg_memory
	reflection_core
	unit_test_lib
)

BW_ADD_TOOL_TEST( generic_plugin_system_unit_test )

BW_PRECOMPILED_HEADER( generic_plugin_system_unit_test pch.hpp )
BW_PROJECT_CATEGORY( generic_plugin_system_unit_test "Unit Tests" )

ADD_DEPENDENCIES( generic_plugin_system_unit_test plg_reflection )
ADD_DEPENDENCIES( generic_plugin_system_unit_test generic_plugin_system_unit_test_plugin1 )
ADD_DEPENDENCIES( generic_plugin_system_unit_test generic_plugin_system_unit_test_plugin2 )

