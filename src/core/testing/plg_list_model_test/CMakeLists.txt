CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_list_model_test )

ADD_DEFINITIONS( -DNOMINMAX )

INCLUDE( WGToolsCoreProject )
INCLUDE( BWQtLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS(
	-D_CRT_SECURE_NO_WARNINGS
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)


SET(RESOURCE_FILES
  controls.qrc
)
source_group("Resource Files" FILES ${RESOURCE_FILES})

qt5_add_resources(COMPILED_RESOURCES ${RESOURCE_FILES})
SOURCE_GROUP("Compiled Resources" FILES ${COMPILED_RESOURCES})

FILE(GLOB_RECURSE QML_SRCS *.qml *.js)


SET( ALL_SRCS
	${QML_SRCS}
	test_list_item.cpp
	test_list_item.hpp
	test_list_model.cpp
	test_list_model.hpp
	plg_list_model_test.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_list_model_test ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_list_model_test PRIVATE
	core_generic_plugin
	core_qt_common
	core_ui_framework )

# Link QT5 modules
qt5_use_modules( plg_list_model_test LINK_PRIVATE Core )

BW_PROJECT_CATEGORY( plg_list_model_test "Plugins" )
