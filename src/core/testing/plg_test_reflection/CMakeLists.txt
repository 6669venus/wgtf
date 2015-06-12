CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_test_reflection )

INCLUDE( BWStandardProject )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

ADD_DEFINITIONS(
	-DCSTDMF_IMPORT
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( METADATA_SRCS
	metadata/test_class.mpp
)
SOURCE_GROUP( "MetaData" FILES ${METADATA_SRCS} )

SET( ALL_SRCS
	plugin_main.cpp
	test_class.cpp
	test_class.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
	${METADATA_SRCS}
)
BW_ADD_TOOL_PLUGIN( plg_test_reflection ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${BLOB_SRCS}
)


BW_TARGET_LINK_LIBRARIES( plg_test_reflection PRIVATE
	generic_plugin
	reflection_core
)

BW_PROJECT_CATEGORY( plg_test_reflection "Plugins" )
