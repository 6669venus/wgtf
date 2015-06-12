CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_copy_paste )

INCLUDE( BWStandardProject )
INCLUDE( BWStandardLibrary )


# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)


SET( ALL_SRCS
	plugin_main.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )
 
BW_ADD_TOOL_PLUGIN( plg_copy_paste ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${ALL_SRCS}
)
 
BW_TARGET_LINK_LIBRARIES( plg_copy_paste PRIVATE
	generic_plugin
	copy_paste )

BW_PROJECT_CATEGORY( plg_copy_paste "Plugins" )
