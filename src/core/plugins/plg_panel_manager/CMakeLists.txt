CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_panel_manager )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )
find_package(QT5)

ADD_DEFINITIONS( -DQT_DECLARATIVE_DEBUG )
ADD_DEFINITIONS( -DQT_QML_DEBUG )

find_package( Qt5Quick REQUIRED )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS(
	-D_CRT_SECURE_NO_WARNINGS
)

SET( ALL_SRCS
	${DISPLAY}
	${DISPLAY_METADATA}
	panel_manager.cpp
	panel_manager.hpp
	plg_panel_manager.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_QT_ADD_QRC(plg_panel_manager resources/plg_panel_manager)
BW_GLOB_RESOURCE_FILES()

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)

BW_ADD_TOOL_PLUGIN( plg_panel_manager SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_panel_manager PRIVATE
	core_generic_plugin
	core_data_model
	core_qt_common
	core_reflection
	core_qt_script
	core_ui_framework
)

# Link QT5 modules
qt5_use_modules( plg_panel_manager LINK_PRIVATE Core Gui Widgets Quick )

BW_PROJECT_CATEGORY( plg_panel_manager "Plugins" )
