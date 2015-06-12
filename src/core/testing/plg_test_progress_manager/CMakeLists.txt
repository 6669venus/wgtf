CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_test_progress_manager )

INCLUDE( BWStandardProject )
INCLUDE( BWQtLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS(
	-DCSTDMF_IMPORT
	-D_CRT_SECURE_NO_WARNINGS
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( COMMAND_SRCS
	commands/test_command.hpp
	commands/test_command.cpp
)
SOURCE_GROUP( "Commands" FILES ${COMMAND_SRCS} )

SOURCE_GROUP("Compiled Resources" FILES ${COMPILED_RESOURCES})

SET( ALL_SRCS
	mainwindow/mainwindow.cpp
	mainwindow/mainwindow.hpp
	plugin_main.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_test_progress_manager ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${ALL_SRCS}
	${COMPILED_RESOURCES}
	${COMMAND_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_test_progress_manager PRIVATE
	command_system
	reflection_core
	generic_plugin
	ui_framework )

# Link QT5 modules
qt5_use_modules( plg_test_progress_manager Core Gui Widgets Quick )

BW_PROJECT_CATEGORY( plg_test_progress_manager "Plugins" )
