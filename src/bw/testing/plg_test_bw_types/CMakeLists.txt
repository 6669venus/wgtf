CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_test_bw_types )

INCLUDE( NGTStandardProject )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

ADD_DEFINITIONS(
	-DCSTDMF_IMPORT
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	plugin_main.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_test_bw_types ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${ALL_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_test_bw_types PRIVATE
	generic_plugin
	qt_common
	reflection_core
	ui_framework
	variant
	data_model
	command_system
	serialization
	bw_adapters )

BW_PROJECT_CATEGORY( plg_test_bw_types "Plugins BW" )
