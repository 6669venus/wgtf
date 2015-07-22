CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_resource_picker )

INCLUDE( BWStandardProject )
INCLUDE( BWQtLibrary )

SET(RESOURCE_FILES
	resources/controls.qrc
	resources/icons.qrc
)
SOURCE_GROUP("Resource Files" FILES ${RESOURCE_FILES})

QT5_ADD_RESOURCES(COMPILED_RESOURCES ${RESOURCE_FILES})
SOURCE_GROUP("Compiled Resources" FILES ${COMPILED_RESOURCES})

SET( ALL_SRCS
	plg_resource_picker.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS 
	${ALL_SRCS}
)
BW_ADD_TOOL_PLUGIN( plg_resource_picker ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_resource_picker PRIVATE
	generic_plugin
	qt_common
	qt_script
	ui_framework
)

# Link QT5 modules
QT5_USE_MODULES( plg_resource_picker Core Gui Widgets Quick )

BW_PROJECT_CATEGORY( plg_resource_picker "Plugins" )

