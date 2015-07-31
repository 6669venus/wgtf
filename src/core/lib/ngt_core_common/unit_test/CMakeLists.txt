CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( ngt_core_common_unit_test )

INCLUDE( WGToolsCoreProject )

SET( ALL_SRCS
	main.cpp
	test_wg_condition_variable.cpp
)

BW_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )
BW_ADD_EXECUTABLE( ngt_core_common_unit_test ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( ngt_core_common_unit_test
	ngt_core_common
	unit_test_lib
)

BW_ADD_TOOL_TEST( ngt_core_common_unit_test )

BW_PROJECT_CATEGORY( ngt_core_common_unit_test "Unit Tests" )

