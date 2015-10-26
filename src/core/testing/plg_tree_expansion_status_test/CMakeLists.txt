CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_tree_expansion_status_test )

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
	tree_list_model.cpp
	tree_list_model.hpp
	tree_list_model.mpp
	plg_tree_expansion_status_test.cpp
)

SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_QT_ADD_QRC(plg_tree_expansion_status_test resources/plg_tree_expansion_status_test)
BW_GLOB_RESOURCE_FILES()

BW_ADD_TOOL_PLUGIN( plg_tree_expansion_status_test SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_tree_expansion_status_test PRIVATE
	core_generic_plugin
	core_qt_common
	core_reflection
	reflection_objects_test
	core_ui_framework )

# Link QT5 modules
qt5_use_modules( plg_tree_expansion_status_test LINK_PRIVATE Core )

BW_PROJECT_CATEGORY( plg_tree_expansion_status_test "Plugins" )
