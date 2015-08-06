CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_window_test )

INCLUDE( WGToolsCoreProject )
INCLUDE( BWQtLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS(
	-D_CRT_SECURE_NO_WARNINGS
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET(RESOURCE_FILES
  resources.qrc
)
source_group("Resource Files" FILES ${RESOURCE_FILES})

qt5_add_resources(COMPILED_RESOURCES ${RESOURCE_FILES})
SOURCE_GROUP("Compiled Resources" FILES ${COMPILED_RESOURCES})

SET( ALL_SRCS
	main_window/main_window.cpp
	main_window/main_window.hpp
	plg_window_test.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_window_test ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_window_test PRIVATE
	core_generic_plugin
	core_qt_common
	core_ui_framework
	)

# Link QT5 modules
qt5_use_modules( plg_window_test LINK_PRIVATE Core )

BW_PROJECT_CATEGORY( plg_window_test "Plugins" )
