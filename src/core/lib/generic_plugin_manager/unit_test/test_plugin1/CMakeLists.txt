CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( gps_unit_test_plg1 )

INCLUDE( NGTStandardProject )
INCLUDE( NGTStandardLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( METADATA_SRCS
	metadata/plugin_objects.mpp
)
SOURCE_GROUP( "MetaData" FILES ${METADATA_SRCS} )

SET( ALL_SRCS
	plugin_main.cpp
	plugin_objects.hpp
	plugin_objects.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
	${METADATA_SRCS}
)
BW_ADD_TOOL_TEST_PLUGIN( gps_unit_test_plg1 SHARED
	${BLOB_SRCS}
)

BW_TARGET_LINK_LIBRARIES( gps_unit_test_plg1 PRIVATE
	generic_plugin
	reflection_core
)

