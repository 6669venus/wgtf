cmake_minimum_required (VERSION 3.0)
project( plg_node_editor_test )

#find_package(NGT REQUIRED)

INCLUDE( WGToolsProject )
INCLUDE( BWQtLibrary )

BW_QT_ADD_QRC(plg_node_editor_test resources)
BW_GLOB_RESOURCE_FILES()

#LIST(APPEND TARGET_LIBRARIES core_generic_plugin core_qt_common core_ui_framework)

#include_directories   ( ${CMAKE_CURRENT_BINARY_DIR})
#include_directories   (${ADDITIONAL_INCLUDES})

add_definitions       (-DNGT_INTEGRATION)

FILE(GLOB_RECURSE QML_SRCS *.qml *.js *.ui *.xml)
SET(QT_RESOURCE_FILES
  ${QML_SRCS}
)

SOURCE_GROUP("Resource Files" FILES ${RESOURCE_FILES} )
SOURCE_GROUP("Compiled Resources" FILES ${COMPILED_RESOURCES})

SET( IMPLEMENTS
	implements/CustomGraph.h
	implements/CustomGraph.cpp
	implements/CustomNode.h
	implements/CustomNode.cpp
	implements/CustomSlot.h
	implements/CustomSlot.cpp
	implements/CustomConnection.h
	implements/CustomConnection.cpp
)
SOURCE_GROUP( "Implements Files" FILES ${IMPLEMENTS} )

SET( ALL_SRCS
	plg_node_editor_test.cpp
)                                     
SOURCE_GROUP( "Source Files" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_node_editor_test SHARED
	${ALL_SRCS}
	${IMPLEMENTS}
)

BW_TARGET_LINK_LIBRARIES( plg_node_editor_test PRIVATE
	core_generic_plugin
	core_qt_common
	core_qt_script
	core_ui_framework
	Qt5::Core
    Qt5::Gui
    Qt5::QuickWidgets
    Qt5::Quick
)

BW_PROJECT_CATEGORY( plg_node_editor_test "Plugins" )

