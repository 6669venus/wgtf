CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( qt_wgs_controls )

INCLUDE( BWStandardProject )
INCLUDE( BWQtLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )
ADD_DEFINITIONS( -DQDESIGNER_EXPORT_WIDGETS )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

FILE(GLOB_RECURSE QML_SRCS *.qml *.js *.ui)
set(QT_RESOURCE_FILES
  ${QML_SRCS}
)
source_group("Qt Resources" FILES ${QT_RESOURCE_FILES})

SET(RESOURCE_FILES
)

SOURCE_GROUP("Resource Files" FILES ${RESOURCE_FILES} )
qt5_add_resources(COMPILED_RESOURCES ${RESOURCE_FILES})
SOURCE_GROUP("Compiled Resources" FILES ${COMPILED_RESOURCES})

SET( ALL_SRCS
	viewport_widget.cpp
	viewport_widget.hpp
	plugins.cpp
	plugins.hpp
)

SOURCE_GROUP( "" FILES ${ALL_SRCS} )
 
BW_ADD_TOOL_PLUGIN( qt_wgs_controls ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${RESOURCE_FILES}
	${QT_RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${ALL_SRCS}
)

# Link QT5 modules
qt5_use_modules( qt_wgs_controls Core Gui Widgets Quick )

BW_PROJECT_CATEGORY( qt_wgs_controls "Plugins" )

#install(TARGETS ${PROJECT}
#    DESTINATION ${QT_PLUGINS_DIR}/designer
#)
