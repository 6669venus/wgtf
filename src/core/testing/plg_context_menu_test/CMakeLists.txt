CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_context_menu_test )

INCLUDE( WGToolsCoreProject )
INCLUDE( BWQtLibrary )

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

qt5_add_resources(COMPILED_RESOURCES ${RESOURCE_FILES})
SOURCE_GROUP("Compiled Resources" FILES ${COMPILED_RESOURCES})

BW_ADD_TOOL_PLUGIN( plg_context_menu_test SHARED
	${ALL_SRCS}
	${COMPILED_RESOURCES}
	${RESOURCE_FILES}
)

BW_TARGET_LINK_LIBRARIES( plg_context_menu_test PRIVATE
	core_generic_plugin
	core_qt_common
	core_ui_framework 
)

# Link QT5 modules
qt5_use_modules( plg_context_menu_test LINK_PRIVATE Core Gui Widgets Quick )

BW_PROJECT_CATEGORY( plg_context_menu_test "Plugins" )
