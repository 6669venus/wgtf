CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_alert_ui )

INCLUDE( BWStandardProject )
INCLUDE( BWStandardLibrary )
INCLUDE( BWQtLibrary )

ADD_DEFINITIONS( -DQT_DECLARATIVE_DEBUG )
ADD_DEFINITIONS( -DQT_QML_DEBUG )

find_package( Qt5Quick REQUIRED )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( 
	-D_CRT_SECURE_NO_WARNINGS 
)

ADD_DEFINITIONS( 
	-DCSTDMF_IMPORT
)

SET( RESOURCE_FILES
	controls.qrc
)
source_group( "Resource Files" FILES ${RESOURCE_FILES} )

qt5_add_resources( COMPILED_RESOURCES ${RESOURCE_FILES} )
SOURCE_GROUP( "Compiled Resources" FILES ${COMPILED_RESOURCES} )

SET( DISPLAY
	alert_models.cpp
	alert_models.hpp
)
SOURCE_GROUP( "Display Page" FILES ${DISPLAY} )

SET( DISPLAY_METADATA
	metadata/alert_models.mpp
)
SOURCE_GROUP( "Display Page\\Metadata" FILES ${DISPLAY_METADATA} )

FILE(GLOB_RECURSE QML_SRCS *.qml *.js)

SET( ALL_SRCS
	${QML_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${DISPLAY}
	${DISPLAY_METADATA}
	plugin_main.cpp
	popup_alert_presenter.cpp
	popup_alert_presenter.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
 
BW_ADD_TOOL_PLUGIN( plg_alert_ui ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_alert_ui PRIVATE
	generic_plugin
	data_model
	qt_common
	reflection_core
	qt_script
	logging_system
	ui_framework
)

# Link QT5 modules
qt5_use_modules( plg_alert_ui LINK_PRIVATE Core Gui Widgets Quick )

BW_PROJECT_CATEGORY( plg_alert_ui "Plugins" )