CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( command_system_unit_test )

INCLUDE( NGTStandardProject )

ADD_DEFINITIONS( 
	-DCSTDMF_IMPORT
)

SET( ALL_SRCS
	main.cpp
	pch.cpp
	pch.hpp
	test_helpers.hpp
	test_command_system_fixture.hpp
	test_command_system_fixture.cpp
	test_command_system.cpp
	test_objects.hpp
	test_objects.cpp
)

BW_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )
BW_ADD_EXECUTABLE( command_system_unit_test ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( command_system_unit_test PRIVATE
	reflection_core
	reflection_utils
	serialization
	command_system
	unit_test_lib
)

BW_ADD_TOOL_TEST( command_system_unit_test )

BW_PRECOMPILED_HEADER( command_system_unit_test pch.hpp )
BW_PROJECT_CATEGORY( command_system_unit_test "Unit Tests" )

