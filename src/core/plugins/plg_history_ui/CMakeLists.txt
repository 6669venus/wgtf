CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_history_ui )

INCLUDE( BWStandardProject )
INCLUDE( BWQtLibrary )

SET( METADATA_SRCS
	metadata/history_object.mpp
    metadata/display_object.mpp
)
SOURCE_GROUP( "MetaData" FILES ${METADATA_SRCS} )


SET( RESOURCE_FILES
	controls.qrc
	icons.qrc
)
SOURCE_GROUP( "Resource Files" FILES ${RESOURCE_FILES} )
qt5_add_resources( COMPILED_RESOURCES ${RESOURCE_FILES} )
SOURCE_GROUP( "Compiled Resources" FILES ${COMPILED_RESOURCES} )

FILE(GLOB_RECURSE QML_SRCS *.qml *.js)

SET( ALL_SRCS
	${QML_SRCS}
	history_object.cpp
	history_object.hpp
    display_object.cpp
    display_object.hpp
	plugin_main.cpp
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
	command_system
	reflection_core
	generic_plugin
)

# Link QT5 modules
qt5_use_modules( plg_history_ui Core )

BW_PROJECT_CATEGORY( plg_history_ui "Plugins" )

