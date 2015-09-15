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
BW_ADD_TOOL_PLUGIN( plg_python27_test ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${BLOB_SRCS}
)
 
BW_TARGET_LINK_LIBRARIES( plg_python27_test PRIVATE
	core_generic_plugin )

BW_PROJECT_CATEGORY( plg_python27_test "Plugins" )

