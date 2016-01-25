CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( plg_maps_settings )

INCLUDE( WGToolsWoWSProject )
INCLUDE( WGPython27Project )

IF( NOT PYTHON_FOUND )
	RETURN()
ENDIF()

FIND_PACKAGE( QT5 )


SET( METADATA_SRCS
    metadata/panel_context.mpp
)
SOURCE_GROUP( "MetaData" FILES ${METADATA_SRCS} )


BW_QT_ADD_QRC( ${PROJECT_NAME} resources/MapsSettings )
BW_GLOB_RESOURCE_FILES()


SET( ALL_SRCS
	balance_panel.hpp
	balance_panel.cpp
	map_status_panel.hpp
	map_status_panel.cpp
	match_group_list_model.hpp
	match_group_list_model.cpp
	match_group_panel.hpp
	match_group_panel.cpp
	panel_context.hpp
	panel_context.cpp
	plg_maps_settings.cpp
	python_loader.hpp
	python_loader.cpp
	python_panel.hpp
	python_panel.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${METADATA_SRCS}
)

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_generic_plugin
	core_python27
	core_qt_common
	core_ui_framework
	Qt5::Core
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "WoWS Plugins" )
