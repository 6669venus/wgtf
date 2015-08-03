CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_idedebug_logger )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( 
	-D_CRT_SECURE_NO_WARNINGS 
)

SET( ALL_SRCS
	plugin_main.cpp
	windows_debug_logger.cpp
	windows_debug_logger.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
 
BW_ADD_TOOL_PLUGIN( plg_idedebug_logger ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${BLOB_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_idedebug_logger PRIVATE
	generic_plugin
	logging_system
)

BW_PROJECT_CATEGORY( plg_idedebug_logger "Plugins" )


