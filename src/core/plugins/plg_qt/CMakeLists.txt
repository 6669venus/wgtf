CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_qt )

INCLUDE( BWStandardProject )
INCLUDE( BWQtLibrary )

set(RESOURCE_FILES
	resources/controls.qrc
	resources/icons.qrc
)
source_group("Resource Files" FILES ${RESOURCE_FILES})

qt5_add_resources(COMPILED_RESOURCES ${RESOURCE_FILES})
SOURCE_GROUP("Compiled Resources" FILES ${COMPILED_RESOURCES})

SET( ALL_SRCS
	plugin_main.cpp
	qt_application.cpp
	qt_application.hpp
	qt_framework.cpp
	qt_framework.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS 
	${ALL_SRCS}
)
BW_ADD_TOOL_PLUGIN( plg_qt ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_qt PRIVATE
	generic_plugin_system
	qt_common
	qt_script
)

# Link QT5 modules
qt5_use_modules( plg_qt Core Gui Widgets Quick )

# Copy the QT5 dlls
# Note: this should not be called from multiple plugins
#	because it may fail if executed concurrently
FUNCTION( BW_COPY_QT_DLLS _PROJNAME _DIR_NAME )
	IF( NOT BW_IS_QT_TOOLS AND NOT BW_IS_GENERIC_APP_TEST)
		MESSAGE( FATAL_ERROR "BW_COPY_QT_DLLS is not supported for non-Qt tools." )
	ENDIF()

	ADD_CUSTOM_COMMAND(	TARGET ${_PROJNAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E echo Copying QT libraries...
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${Qt5_DIR}/qml/QtQuick/ $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick/
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${Qt5_DIR}/qml/QtQuick.2/ $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick.2/
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${Qt5_DIR}/qml/QtGraphicalEffects/ $<TARGET_FILE_DIR:${_DIR_NAME}>/QtGraphicalEffects/
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${Qt5_DIR}/plugins/qmltooling/ $<TARGET_FILE_DIR:${_DIR_NAME}>/qmltooling/
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${Qt5Plugins_DIR}/platforms/ $<TARGET_FILE_DIR:${_DIR_NAME}>/platforms/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5Core.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5Cored.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5Cored.pdb $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5Gui.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5Guid.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5Guid.pdb $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5Network.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5Networkd.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5Networkd.pdb $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5Qml.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5Qmld.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5Qmld.pdb $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5Quick.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5Quickd.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5Quickd.pdb $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5Widgets.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5Widgetsd.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5Widgetsd.pdb $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/icudt53.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/icuin53.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/icuuc53.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/libEGL.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/libEGLd.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/libEGLd.pdb $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/libGLESv2.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/libGLESv2d.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/libGLESv2d.pdb $<TARGET_FILE_DIR:${_DIR_NAME}>/
	)
ENDFUNCTION()
BW_COPY_QT_DLLS( plg_qt generic_app )

FUNCTION( BW_COPY_OTHER_RESOURCES _PROJNAME _DIR_NAME )
	ADD_CUSTOM_COMMAND( TARGET ${_PROJNAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/resources/fonts/Apache License.txt" $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/resources/fonts/NotoSans-Bold.ttf $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/resources/fonts/NotoSans-BoldItalic.ttf $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/resources/fonts/NotoSans-Italic.ttf $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/resources/fonts/NotoSans-Regular.ttf $<TARGET_FILE_DIR:${_DIR_NAME}>/
	)
ENDFUNCTION()
BW_COPY_OTHER_RESOURCES( plg_qt generic_app )

BW_PROJECT_CATEGORY( plg_qt "Plugins" )

