CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( generic_plugin_manager )

INCLUDE( WGToolsProject )
INCLUDE( WGToolsLibrary )

SET( ALL_SRCS
	config_plugin_loader.hpp
	config_plugin_loader.cpp
	default_context_manager.hpp
	default_context_manager.cpp
	folder_plugin_loader.hpp
	folder_plugin_loader.cpp
	generic_plugin_manager.hpp
	generic_plugin_manager.cpp
	plugin_context_manager.hpp
	plugin_context_manager.cpp
	notify_plugin.hpp
	notify_plugin.cpp
)
SOURCE_GROUP( "Source" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( generic_plugin_manager ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( generic_plugin_manager INTERFACE
	ngt_core_common
	variant
	dependency_system
	logging

	# external libraries
	shlwapi
)

BW_PROJECT_CATEGORY( generic_plugin_manager "Plugin System" )

