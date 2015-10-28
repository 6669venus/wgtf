CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_demo_test )

INCLUDE( WGToolsCoreProject )

find_package(QT5)
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	plg_demo_test.cpp
	tinyxml2.cpp
	tinyxml2.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_QT_ADD_QRC(plg_demo_test resources/plg_demo_test)
BW_GLOB_RESOURCE_FILES()

BW_ADD_TOOL_PLUGIN( plg_demo_test SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
	resources/sceneModel.xml
)

BW_TARGET_LINK_LIBRARIES( plg_demo_test PRIVATE
	core_generic_plugin
	core_qt_common
	core_ui_framework
    Qt5::Core
	)

FILE( GLOB modelFiles resources/sceneModel.xml )
BW_CUSTOM_COPY_TO_PROJECT_OUTPUT( .. "${modelFiles}" )

BW_PROJECT_CATEGORY( plg_demo_test "Plugins" )
