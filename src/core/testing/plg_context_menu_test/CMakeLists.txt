CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_context_menu_test )

INCLUDE( WGToolsCoreProject )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	plg_context_menu_test.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

SET(RESOURCE_FILES
  resources.qrc
)
source_group("Resource Files" FILES ${RESOURCE_FILES})

BW_ADD_TOOL_PLUGIN( plg_context_menu_test ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
)

BW_TARGET_LINK_LIBRARIES( plg_context_menu_test PRIVATE
	core_generic_plugin )

BW_PROJECT_CATEGORY( plg_context_menu_test "Plugins" )
