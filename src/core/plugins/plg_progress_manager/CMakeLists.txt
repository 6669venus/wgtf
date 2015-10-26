CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_progress_manager )

INCLUDE( WGToolsCoreProject )
find_package(QT5)

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS(
	-D_CRT_SECURE_NO_WARNINGS
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	plg_progress_manager.cpp
	progress_manager.hpp
	progress_manager.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_QT_ADD_QRC(WGProgressManager resources/WGProgressManager)
BW_GLOB_RESOURCE_FILES()



BW_ADD_TOOL_PLUGIN( plg_progress_manager SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_progress_manager PRIVATE
	core_generic_plugin
	core_command_system
	core_qt_common
	core_reflection
	core_qt_script )

# Link QT5 modules
qt5_use_modules( plg_progress_manager LINK_PRIVATE Core Gui Widgets Quick )

BW_PROJECT_CATEGORY( plg_progress_manager "Plugins" )
