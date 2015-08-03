CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_asset_browser_test )

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
	models/test_asset_browser_models.cpp
	models/test_asset_browser_models.hpp
)
SOURCE_GROUP( "Display Page" FILES ${DISPLAY} )

SET( DISPLAY_METADATA
	metadata/test_asset_browser_models.mpp
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
	test_asset_browser_manager.cpp
	test_asset_browser_manager.hpp
	plugin_main.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
 
BW_ADD_TOOL_PLUGIN( plg_asset_browser_test ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_asset_browser_test PRIVATE
	core_generic_plugin
	core_data_model
	core_qt_common
	core_reflection
	core_qt_script
	core_ui_framework
)

# Link QT5 modules
qt5_use_modules( plg_asset_browser_test LINK_PRIVATE Core Gui Widgets Quick )

BW_PROJECT_CATEGORY( plg_asset_browser_test "Plugins" )
