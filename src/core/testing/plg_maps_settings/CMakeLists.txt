CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( plg_maps_settings )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGPython27Project )

IF( NOT PYTHON_FOUND )
	RETURN()
ENDIF()

find_package(QT5)

# Find includes in corresponding build directories
set( CMAKE_INCLUDE_CURRENT_DIR ON )

SET( PYTHON_FILES
	scripts/MapsSettingsEditor/DataStructures.py
	scripts/MapsSettingsEditor/Paths.py
	scripts/MapsSettingsEditor/ResourceDeserializers.py
)
SOURCE_GROUP( "Python" FILES ${PYTHON_FILES} )

SET( QML_FILES
	resources/qml/maps_settings_panel.qml
)
SOURCE_GROUP( "QML" FILES ${QML_FILES} )

SET( ALL_SRCS
	python_panel.hpp
	python_panel.cpp
	plg_maps_settings.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)

BW_QT_ADD_QRC( ${PROJECT_NAME} resources/qml )
BW_GLOB_RESOURCE_FILES()

BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${QML_FILES}
	${PYTHON_FILES}
)

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_generic_plugin
	core_qt_common
	core_ui_framework
	Qt5::Core
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
