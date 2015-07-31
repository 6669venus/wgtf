CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( generic_plugin )

INCLUDE( NGTStandardProject )
INCLUDE( NGTStandardLibrary )

SET( INTERFACE_SRCS 
	interfaces/i_application.hpp
	interfaces/i_component_context.hpp
	interfaces/i_memory_allocator.hpp
	interfaces/i_component_context_creator.hpp
	interfaces/i_plugin_context_manager.hpp
	interfaces/ui_connection.hpp
)
SOURCE_GROUP( "Interfaces" FILES ${INTERFACE_SRCS} )

SET( ALL_SRCS
	generic_plugin.hpp
	generic_plugin.cpp
)
SOURCE_GROUP( "Source" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS
	${INTERFACE_SRCS}
	${ALL_SRCS}
)
BW_ADD_LIBRARY( generic_plugin ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( generic_plugin INTERFACE
	ngt_core_common
	variant
	dependency_system
	logging
)

BW_PROJECT_CATEGORY( generic_plugin "Plugin System" )

