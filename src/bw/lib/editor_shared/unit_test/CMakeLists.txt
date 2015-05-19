CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( editor_shared_unit_test )

INCLUDE( BWStandardProject )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/ngt/lib )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/third_party )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/ngt/tools )

ADD_DEFINITIONS( 
	-DCSTDMF_IMPORT
)

SET( ALL_SRCS
	main.cpp
	pch.cpp
	pch.hpp
	test_text_utilities.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )
BW_ADD_EXECUTABLE( editor_shared_unit_test ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( editor_shared_unit_test PRIVATE
	string_utils
	unit_test_lib
)

BW_ADD_TOOL_TEST( editor_shared_unit_test )

BW_PRECOMPILED_HEADER( editor_shared_unit_test pch.hpp )
BW_PROJECT_CATEGORY( editor_shared_unit_test "Unit Tests" )

