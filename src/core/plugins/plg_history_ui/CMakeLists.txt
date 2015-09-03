CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_history_ui )

INCLUDE( WGToolsCoreProject )
INCLUDE( BWQtLibrary )

SET( METADATA_SRCS
	metadata/history_object.mpp
    metadata/display_object.mpp
)
SOURCE_GROUP( "MetaData" FILES ${METADATA_SRCS} )


BW_QT_ADD_QRC(WGHistory resources/WGHistory)
BW_GLOB_RESOURCE_FILES()


SET( ALL_SRCS
	${QML_SRCS}
	history_object.cpp
	history_object.hpp
    display_object.cpp
    display_object.hpp
	plg_history_ui.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
	${METADATA_SRCS}
)
BW_ADD_TOOL_PLUGIN( plg_history_ui ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)


BW_TARGET_LINK_LIBRARIES( plg_history_ui PRIVATE
	core_command_system
	core_reflection
	core_qt_common
	core_generic_plugin
)

# Link QT5 modules
qt5_use_modules( plg_history_ui LINK_PRIVATE Core Quick )

BW_PROJECT_CATEGORY( plg_history_ui "Plugins" )

