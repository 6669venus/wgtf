CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( generic_app )

INCLUDE( WGToolsCoreProject )
INCLUDE( BWProjectUserFile )

SET( APP_CODE_SRCS
	app/main.cpp
	app/plugin_memory_allocator.hpp
	app/plugin_memory_allocator.cpp
	app/memory_plugin_context_creator.hpp
	app/memory_plugin_context_creator.cpp
)
SOURCE_GROUP( "App" FILES ${APP_CODE_SRCS} )

FILE(GLOB PLUGIN_CONFIG_FILES ${NGT_PLUGIN_CONFIGURATION_FILES})
SOURCE_GROUP( "Plugin Configurations" FILES ${PLUGIN_CONFIG_FILES} )

BW_BLOB_SOURCES( BLOB_SRCS
	${APP_CODE_SRCS}
	${PLUGIN_CONFIG_FILES}
)
BW_ADD_TOOL_EXE( generic_app ${TARGET_PLUGIN_APP_FOLDER_NAME} WIN32 ${BLOB_SRCS} )


BW_TARGET_LINK_LIBRARIES( generic_app PRIVATE
	generic_plugin_manager
	wg_memory

	# external libraries
	shlwapi
)

BW_CUSTOM_COPY_TO_PROJECT_OUTPUT( plugins "${PLUGIN_CONFIG_FILES}" )

BW_PROJECT_CATEGORY( generic_app "Executables" )

