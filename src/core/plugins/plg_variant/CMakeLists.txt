CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_variant )

INCLUDE( BWStandardProject )
INCLUDE( BWStandardLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	plg_variant.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_variant ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${ALL_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_variant PRIVATE
	core_generic_plugin
	core_variant )

BW_PROJECT_CATEGORY( plg_variant "Plugins" )
