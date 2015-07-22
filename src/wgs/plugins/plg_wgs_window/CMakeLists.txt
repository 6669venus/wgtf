CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_wgs_window )

INCLUDE( BWStandardProject )
INCLUDE( BWQtLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( 
	-DCSTDMF_IMPORT
	-D_CRT_SECURE_NO_WARNINGS
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET(RESOURCE_FILES
  resources/resources.qrc
)
source_group("Resource Files" FILES ${RESOURCE_FILES})

qt5_add_resources(COMPILED_RESOURCES ${RESOURCE_FILES})
SOURCE_GROUP("Compiled Resources" FILES ${COMPILED_RESOURCES})

SET( ALL_SRCS
	window_manager.cpp
	window_manager.hpp
	plugin_main.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )
 
BW_ADD_TOOL_PLUGIN( plg_wgs_window ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)
 
BW_TARGET_LINK_LIBRARIES( plg_wgs_window PRIVATE
	generic_plugin
	qt_common
	ui_framework
	)

# Link QT5 modules
qt5_use_modules( plg_wgs_window Core )

BW_PROJECT_CATEGORY( plg_wgs_window "Plugins" )
