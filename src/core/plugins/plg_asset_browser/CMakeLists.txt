CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_asset_browser )

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
	models/asset_browser_page_model.cpp
	models/asset_browser_page_model.hpp
	models/folder_content_object_model.cpp
	models/folder_content_object_model.hpp
	models/folder_tree_item.cpp
	models/folder_tree_item.hpp
	models/folder_tree_model.cpp
	models/folder_tree_model.hpp
)
SOURCE_GROUP( "Display Page" FILES ${DISPLAY} )

SET( DISPLAY_METADATA
	metadata/asset_browser_models.mpp
)
SOURCE_GROUP( "Display Page\\Metadata" FILES ${DISPLAY_METADATA} )

SET( DISPLAY_CUSTOM_FILTER
	filters/asset_browser_list_custom_filter.hpp
	filters/asset_browser_list_custom_filter.cpp
)
SOURCE_GROUP( "Display Page\\Filters" FILES ${DISPLAY_CUSTOM_FILTER} )

FILE(GLOB_RECURSE QML_SRCS *.qml *.js)


SET( ALL_SRCS
	${QML_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${DISPLAY}
	${DISPLAY_METADATA}
	${DISPLAY_CUSTOM_FILTER}
	asset_browser_manager.cpp
	asset_browser_manager.hpp
	plugin_main.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
 
BW_ADD_TOOL_PLUGIN( plg_asset_browser ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_asset_browser PRIVATE
	generic_plugin
	data_model
	qt_common
	reflection_core
	qt_script
	ui_framework
)

# Link QT5 modules
qt5_use_modules( plg_asset_browser LINK_PRIVATE Core Gui Widgets Quick )

BW_PROJECT_CATEGORY( plg_asset_browser "Plugins" )
