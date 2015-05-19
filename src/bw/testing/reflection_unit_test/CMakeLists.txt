CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( reflection_unit_test )

INCLUDE( BWStandardProject )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/ngt/lib )

ADD_DEFINITIONS( 
	-DCSTDMF_IMPORT
)

SET( ALL_SRCS
	main.cpp
	pch.hpp
	pch.cpp
	test_reflection_fixture.hpp
	test_reflection_fixture.cpp
	test_property_accessor.cpp
	test_class_definition.cpp
	test_function_property.cpp
	test_generic_object.cpp
	test_helpers.hpp
	test_objects.hpp
	test_objects.cpp
	test_reflected_property.cpp
	test_reflection_utilities.cpp
)

BW_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )
BW_ADD_EXECUTABLE( reflection_unit_test ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( reflection_unit_test
	reflection_core
	reflection_test_objects
	serialization
	unit_test_lib
)

BW_ADD_TOOL_TEST( reflection_unit_test )

BW_PRECOMPILED_HEADER( reflection_unit_test pch.hpp )
BW_PROJECT_CATEGORY( reflection_unit_test "Unit Tests" )

