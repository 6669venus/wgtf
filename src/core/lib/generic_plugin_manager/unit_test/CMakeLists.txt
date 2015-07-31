CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( gps_unit_test )

INCLUDE( BWStandardProject )

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
BW_ADD_EXECUTABLE( gps_unit_test ${BLOB_SRCS} )
BW_TARGET_LINK_LIBRARIES( gps_unit_test PRIVATE
	generic_plugin_manager
	wg_memory
	reflection_core
	unit_test_lib
)

BW_ADD_TOOL_TEST( gps_unit_test )

BW_PRECOMPILED_HEADER( gps_unit_test pch.hpp )
BW_PROJECT_CATEGORY( gps_unit_test "Unit Tests" )

ADD_DEPENDENCIES( gps_unit_test plg_reflection )
ADD_DEPENDENCIES( gps_unit_test gps_unit_test_plg1 )
ADD_DEPENDENCIES( gps_unit_test gps_unit_test_plg2 )

