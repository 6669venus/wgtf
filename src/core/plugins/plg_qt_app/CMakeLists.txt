CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_qt_app )

INCLUDE( BWStandardProject )
INCLUDE( BWQtLibrary )

SET( ALL_SRCS
	plugin_main.cpp
	qt_application.cpp
	qt_application.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${BLOB_SRCS}
)

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	ngt_core_common
	generic_plugin
	qt_common
	qt_script
	ui_framework
)

# Link QT5 modules
qt5_use_modules( ${PROJECT_NAME} LINK_PRIVATE Core Gui Widgets Quick )

IF( BW_IS_GENERIC_APP_TEST OR BW_IS_QT_TOOLS )
	BW_DEPLOY_QT( generic_app )
ENDIF()

# Avoid conflicts when deploy Qt binaries between plg_qt_app and plg_qt_common
ADD_DEPENDENCIES( ${PROJECT_NAME} plg_qt_common )

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
