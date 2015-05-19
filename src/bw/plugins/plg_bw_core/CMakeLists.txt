CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_bw_core )

INCLUDE( BWStandardProject )
INCLUDE( BWStandardLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)


SET( SERIALIZER
	serializer/bw_type_serializer.hpp
	serializer/bw_type_serializer.cpp
)
SOURCE_GROUP( "serializer" FILES ${SERIALIZER} )

SET( ALL_SRCS
	plugin_main.cpp
	qt_type_converters.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )
 
BW_ADD_TOOL_PLUGIN( plg_bw_core ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${ALL_SRCS}
	${SERIALIZER}
)
 
BW_TARGET_LINK_LIBRARIES( plg_bw_core PRIVATE
	generic_plugin_system
	editor_shared
	data_model
	bw_adapters
	 )

# Link QT5 modules
qt5_use_modules( plg_bw_core LINK_PRIVATE Core Gui )

BW_PROJECT_CATEGORY( plg_bw_core "Plugins BW" )
