CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_automation )

INCLUDE( BWStandardProject )
INCLUDE( BWStandardLibrary )

SET( ALL_SRCS
	plugin_main.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_automation ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${ALL_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_automation PRIVATE
	cstdmf
	generic_plugin
)

BW_PROJECT_CATEGORY( plg_automation "Plugins" )

