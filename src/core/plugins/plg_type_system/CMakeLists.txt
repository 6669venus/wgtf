CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_type_system )

INCLUDE( BWStandardProject )
INCLUDE( BWStandardLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	plugin_main.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_type_system ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${ALL_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_type_system PRIVATE
	core_generic_plugin
	core_variant )

BW_PROJECT_CATEGORY( plg_type_system "Plugins" )
