CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_reflection_object_test )

ADD_DEFINITIONS( -DNOMINMAX )

INCLUDE( WGToolsCoreProject )
INCLUDE( BWQtLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS(
	-D_CRT_SECURE_NO_WARNINGS
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)


SET( ALL_SRCS
	${QML_SRCS}
	plg_reflection_object_test.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_reflection_object_test ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${ALL_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_reflection_object_test PRIVATE
	core_generic_plugin
	core_reflection
	reflection_objects_test )

# Link QT5 modules
qt5_use_modules( plg_reflection_object_test LINK_PRIVATE Core )

BW_PROJECT_CATEGORY( plg_reflection_object_test "Plugins" )
