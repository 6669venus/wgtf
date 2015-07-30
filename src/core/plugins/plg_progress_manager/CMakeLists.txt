CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_progress_manager )

INCLUDE( BWStandardProject )
INCLUDE( BWQtLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS(
	-D_CRT_SECURE_NO_WARNINGS
)

ADD_DEFINITIONS(
	-DCSTDMF_IMPORT
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(RESOURCE_FILES
  controls.qrc
)
source_group("Resource Files" FILES ${RESOURCE_FILES})

qt5_add_resources(COMPILED_RESOURCES ${RESOURCE_FILES})
SOURCE_GROUP("Compiled Resources" FILES ${COMPILED_RESOURCES})

FILE(GLOB_RECURSE QML_SRCS *.qml *.js)


SET( ALL_SRCS
	${QML_SRCS}
	plugin_main.cpp
	progress_manager.hpp
	progress_manager.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_progress_manager ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_progress_manager PRIVATE
	generic_plugin
	command_system
	reflection_core
	qt_script )

# Link QT5 modules
qt5_use_modules( plg_progress_manager LINK_PRIVATE Core Gui Widgets Quick )

BW_PROJECT_CATEGORY( plg_progress_manager "Plugins" )
