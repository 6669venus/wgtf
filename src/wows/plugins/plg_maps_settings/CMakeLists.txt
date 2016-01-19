CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( plg_maps_settings )

INCLUDE( WGToolsWoWSProject )
INCLUDE( WGPython27Project )

IF( NOT PYTHON_FOUND )
	RETURN()
ENDIF()

FIND_PACKAGE( QT5 )


SET( METADATA_SRCS
    metadata/balance_panel_context.mpp
)
SOURCE_GROUP( "MetaData" FILES ${METADATA_SRCS} )


BW_QT_ADD_QRC( ${PROJECT_NAME} resources/MapsSettings )
BW_GLOB_RESOURCE_FILES()


SET( ALL_SRCS
	balance_panel.hpp
	balance_panel.cpp
	balance_panel_context.hpp
	balance_panel_context.cpp
	map_status_panel.hpp
	map_status_panel.cpp
	plg_maps_settings.cpp
	python_panel.hpp
	python_panel.cpp
	match_group_list_model.hpp
	match_group_list_model.cpp
	pvp_panel.hpp
	pvp_panel.cpp
	pvp_ranked_panel.hpp
	pvp_ranked_panel.cpp
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
	core_qt_common
	core_ui_framework
	Qt5::Core
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "WoWS Plugins" )
