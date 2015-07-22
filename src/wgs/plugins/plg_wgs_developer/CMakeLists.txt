CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_wgs_developer )

INCLUDE( BWStandardProject )
INCLUDE( BWQtLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( 
	-DCSTDMF_IMPORT
	-D_CRT_SECURE_NO_WARNINGS
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

FILE(GLOB_RECURSE QML_SRCS *.qml *.js *.ui)
set(QT_RESOURCE_FILES
  ${QML_SRCS}
)
source_group("Qt Resources" FILES ${QT_RESOURCE_FILES})

SET(RESOURCE_FILES
  resources.qrc
)

SOURCE_GROUP("Resource Files" FILES ${RESOURCE_FILES} )
qt5_add_resources(COMPILED_RESOURCES ${RESOURCE_FILES})
SOURCE_GROUP("Compiled Resources" FILES ${COMPILED_RESOURCES})

SET( ALL_SRCS
	plg_wgs_developer.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )
 
BW_ADD_TOOL_PLUGIN( plg_wgs_developer ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${QT_RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${ALL_SRCS}
)
 
BW_TARGET_LINK_LIBRARIES( plg_wgs_developer PRIVATE
	generic_plugin
	qt_common
	ui_framework
	)

# Link QT5 modules
qt5_use_modules( plg_wgs_developer Core Gui Widgets Quick )

BW_PROJECT_CATEGORY( plg_wgs_developer "Plugins" )
