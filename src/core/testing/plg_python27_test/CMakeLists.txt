CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( plg_python27_test )

INCLUDE( WGToolsCoreProject )

SET( ALL_SRCS
	plg_python27_test.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_TOOL_PLUGIN( plg_python27_test SHARED
	${BLOB_SRCS}
)
 
BW_TARGET_LINK_LIBRARIES( plg_python27_test PRIVATE
	core_generic_plugin
	core_reflection
)

BW_PROJECT_CATEGORY( plg_python27_test "Plugins" )

