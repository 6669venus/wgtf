CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_wgs_blueprint_editor )

ADD_DEFINITIONS( -DNOMINMAX )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/bw )
INCLUDE( BWStandardProject )
INCLUDE( BWQtLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS(
	-DCSTDMF_IMPORT
	-D_CRT_SECURE_NO_WARNINGS
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( METADATA
	metadata/blueprint_model.mpp
)
SOURCE_GROUP( "Metadata" FILES ${METADATA} )

SET( MODELS
	models/blueprint_model.hpp
	models/blueprint_model.cpp
)
SOURCE_GROUP( "Models" FILES ${MODELS} )

SET( INTERFACES
	interfaces/i_datasource.hpp
)
SOURCE_GROUP( "Interfaces" FILES ${INTERFACES} )

SET(RESOURCE_FILES
  resources/controls.qrc
)
source_group("Resource Files" FILES ${RESOURCE_FILES})

qt5_add_resources(COMPILED_RESOURCES ${RESOURCE_FILES})
SOURCE_GROUP("Compiled Resources" FILES ${COMPILED_RESOURCES})

FILE(GLOB_RECURSE QML_SRCS *.qml *.js)


SET( ALL_SRCS
	${QML_SRCS}
	blueprint_editor_manager.cpp
	blueprint_editor_manager.hpp
	blueprint_datasource.cpp
	blueprint_datasource.hpp
	blueprint_object.cpp
	blueprint_object.hpp
	plugin_main.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_wgs_blueprint_editor ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${INTERFACES}
	${MODELS}
	${METADATA}
)

BW_TARGET_LINK_LIBRARIES( plg_wgs_blueprint_editor PRIVATE
	generic_plugin
	qt_common
	reflection
	command_system
	serialization
	ui_framework )

# Link QT5 modules
qt5_use_modules( plg_wgs_blueprint_editor Core Gui Widgets Quick )

BW_PROJECT_CATEGORY( plg_wgs_blueprint_editor "Plugins" )
