CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_qt_app )

INCLUDE( BWStandardProject )
INCLUDE( BWQtLibrary )

SET( ALL_SRCS
	plg_qt_app.cpp
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
	core_common
	core_generic_plugin
	core_qt_common
	core_qt_script
	core_ui_framework
)

# Link QT5 modules
qt5_use_modules( ${PROJECT_NAME} LINK_PRIVATE Core Gui Widgets Quick )

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
