CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_editor_interaction )

INCLUDE( NGTStandardProject )
INCLUDE( NGTStandardLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)


SET( ALL_SRCS
	plugin_main.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_editor_interaction ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${ALL_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_editor_interaction PRIVATE
	generic_plugin
	reflection_core
	reflection_utils
	command_system )

BW_PROJECT_CATEGORY( plg_editor_interaction "Plugins" )
