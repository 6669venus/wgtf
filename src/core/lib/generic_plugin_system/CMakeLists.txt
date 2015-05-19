CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( generic_plugin_system )

INCLUDE( BWStandardProject )
INCLUDE( BWStandardLibrary )

SET( INTERFACE_SRCS 
	interfaces/i_application.hpp
	interfaces/i_context_manager.hpp
	interfaces/i_memory_allocator.hpp
	interfaces/i_plugin_context_creator.hpp
	interfaces/ui_connection.hpp
)
SOURCE_GROUP( "Interfaces" FILES ${INTERFACE_SRCS} )

SET( ALL_SRCS
	default_context_manager.hpp
	default_context_manager.cpp
	generic_plugin.hpp
	generic_plugin.cpp
	generic_plugin_manager.hpp
	generic_plugin_manager.cpp
	config_plugin_loader.hpp
	config_plugin_loader.cpp
	folder_plugin_loader.hpp
	folder_plugin_loader.cpp
)
SOURCE_GROUP( "Source" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS
	${INTERFACE_SRCS}
	${ALL_SRCS}
)
BW_ADD_LIBRARY( generic_plugin_system ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( generic_plugin_system INTERFACE
	variant
	dependency_system
	logging

	# external libraries
	shlwapi
)

BW_PROJECT_CATEGORY( generic_plugin_system "Plugin System" )

