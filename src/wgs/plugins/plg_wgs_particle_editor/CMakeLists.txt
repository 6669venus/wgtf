CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_wgs_particle_editor )

INCLUDE( BWStandardProject )
INCLUDE( BWQtLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( 
	-DCSTDMF_IMPORT
	-D_CRT_SECURE_NO_WARNINGS
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

FILE(GLOB_RECURSE QML_SRCS *.qml *.js *.ui *.xml)
set(QT_RESOURCE_FILES
  ${QML_SRCS}
)

# feels like there should be a better was of setting up dependencies, but unfortunatly it appears
# that this is the only way of doing it :(
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/wgs/plugins/plg_wgs_core)

source_group("Qt Resources" FILES ${QT_RESOURCE_FILES})

SET(RESOURCE_FILES
  resources.qrc
)

SOURCE_GROUP("Resource Files" FILES ${RESOURCE_FILES} )
qt5_add_resources(COMPILED_RESOURCES ${RESOURCE_FILES})
SOURCE_GROUP("Compiled Resources" FILES ${COMPILED_RESOURCES})

SET( ALL_SRCS
	main_window/main_window.cpp
	main_window/main_window.hpp
	plugin_main.cpp
	particle_node_tree_item.cpp
	particle_node_tree_item.hpp
	particle_node_tree_model.cpp
	particle_node_tree_model.hpp
	interfaces/i_datasource.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )
 
BW_ADD_TOOL_PLUGIN( plg_wgs_particle_editor ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${RESOURCE_FILES}
	${QT_RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${ALL_SRCS}
)
 
BW_TARGET_LINK_LIBRARIES( plg_wgs_particle_editor PRIVATE
	generic_plugin
	qt_common
	ui_framework
)

# Link QT5 modules
qt5_use_modules( plg_wgs_particle_editor Core Gui Widgets Quick )

BW_PROJECT_CATEGORY( plg_wgs_particle_editor "Plugins" )
