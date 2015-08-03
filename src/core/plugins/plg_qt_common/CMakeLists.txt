CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_qt_common )

INCLUDE( BWStandardProject )
INCLUDE( BWQtLibrary )

set( RESOURCE_FILES
	resources/controls.qrc
	resources/icons.qrc
)

FILE( GLOB_RECURSE JS_SRCS *.js )

FILE( GLOB_RECURSE QML_SRCS *.qml *.js qmldir )

source_group( "Resource Files" FILES ${RESOURCE_FILES} )

qt5_add_resources( COMPILED_RESOURCES ${RESOURCE_FILES} )
SOURCE_GROUP( "Compiled Resources" FILES ${COMPILED_RESOURCES} )

SET( ALL_SRCS
	${QML_SRCS}
	${JS_SRCS}
	plugin_main.cpp
	qt_framework.cpp
	qt_framework.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_ngt_common
	core_generic_plugin
	core_qt_common
	core_qt_script
	core_ui_framework
)

# Link QT5 modules
qt5_use_modules( ${PROJECT_NAME} LINK_PRIVATE Core Gui QuickWidgets Quick )

FILE( GLOB resFiles resources/fonts/* )
BW_DEPLOY_RESOURCES( .. "${resFiles}" )

IF( BW_IS_GENERIC_APP_TEST OR BW_IS_QT_TOOLS )
	BW_DEPLOY_QT( generic_app )
ENDIF()

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )

