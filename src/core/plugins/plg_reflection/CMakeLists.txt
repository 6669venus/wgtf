CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_reflection )

INCLUDE( BWStandardProject )
INCLUDE( BWStandardLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)


SET( ALL_SRCS
	context_definition_manager.cpp
	context_definition_manager.hpp 
	plugin_main.cpp
	reflection_component_provider.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )
 
BW_ADD_TOOL_PLUGIN( plg_reflection ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${ALL_SRCS}
)

# Must be copied to unit test bin
BW_COPY_PLUGIN_TO_UNIT_TEST_BIN( plg_reflection )
 
BW_TARGET_LINK_LIBRARIES( plg_reflection PRIVATE
	generic_plugin_system
	reflection_core
	reflection_utils
)

BW_PROJECT_CATEGORY( plg_reflection "Plugins" )
