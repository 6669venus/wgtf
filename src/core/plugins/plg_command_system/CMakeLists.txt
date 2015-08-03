CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_command_system )

INCLUDE( BWStandardProject )

REMOVE_DEFINITIONS( -WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)


SET( ALL_SRCS
	plugin_main.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_command_system ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${ALL_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_command_system PRIVATE
	core_reflection
	core_generic_plugin
	core_command_system
	core_serialization
)

BW_PROJECT_CATEGORY( plg_command_system "Plugins" )
