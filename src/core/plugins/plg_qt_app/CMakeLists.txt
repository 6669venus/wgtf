CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_qt_app )

INCLUDE( BWStandardProject )
INCLUDE( BWQtLibrary )

SET( ALL_SRCS
	plugin_main.cpp
	qt_application.cpp
	qt_application.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${BLOB_SRCS}
)

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	generic_plugin
	qt_common
	qt_script
	ui_framework
)

# Link QT5 modules
qt5_use_modules( ${PROJECT_NAME} Core Gui Widgets Quick )

# Copy the QT5 dlls
# Note: this should not be called from multiple plugins
#	because it may fail if executed concurrently
FUNCTION( BW_COPY_QT_DLLS _PROJNAME _DIR_NAME )
	ADD_CUSTOM_COMMAND(	TARGET ${_PROJNAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E echo Copying QT libraries...
		COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick/Controls/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5_DIR}/qml/QtQuick/Controls/qtquickcontrolsplugin.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick/Controls/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5_DIR}/qml/QtQuick/Controls/qtquickcontrolsplugind.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick/Controls/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5_DIR}/qml/QtQuick/Controls/qtquickcontrolsplugind.pdb $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick/Controls/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5_DIR}/qml/QtQuick/Controls/qmldir $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick/Controls/
		COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick/Dialogs/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5_DIR}/qml/QtQuick/Dialogs/dialogplugin.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick/Dialogs/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5_DIR}/qml/QtQuick/Dialogs/dialogplugind.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick/Dialogs/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5_DIR}/qml/QtQuick/Dialogs/dialogplugind.pdb $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick/Dialogs/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5_DIR}/qml/QtQuick/Dialogs/qmldir $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick/Dialogs/
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${Qt5_DIR}/qml/QtQuick/Layouts/ $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick/Layouts/
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${Qt5_DIR}/qml/QtQuick/LocalStorage/ $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick/LocalStorage/
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${Qt5_DIR}/qml/QtQuick/Particles.2/ $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick/Particles.2/
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${Qt5_DIR}/qml/QtQuick/PrivateWidgets/ $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick/PrivateWidgets/
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${Qt5_DIR}/qml/QtQuick/Window.2/ $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick/Window.2/
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${Qt5_DIR}/qml/QtQuick/XmlListModel/ $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick/XmlListModel/
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
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5QuickWidgets.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5QuickWidgetsd.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5QuickWidgetsd.pdb $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5Xml.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5Xmld.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/Qt5Xmld.pdb $<TARGET_FILE_DIR:${_DIR_NAME}>/
	)

	FILE( GLOB QT5_ICU_FILES ${Qt5Bin_DIR}/icu*.dll )
	FOREACH (_FILE ${QT5_ICU_FILES})
		ADD_CUSTOM_COMMAND(	TARGET ${_PROJNAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E echo Copying ICU ${_FILE} file...
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_FILE} $<TARGET_FILE_DIR:${_DIR_NAME}>/
		)
	ENDFOREACH()

	IF( EXISTS ${Qt5Bin_DIR}/libEGL.dll AND 
		EXISTS ${Qt5Bin_DIR}/libEGLd.dll AND 
		EXISTS ${Qt5Bin_DIR}/libEGLd.pdb AND 
		EXISTS ${Qt5Bin_DIR}/libGLESv2.dll AND 
		EXISTS ${Qt5Bin_DIR}/libGLESv2d.dll AND 
		EXISTS ${Qt5Bin_DIR}/libGLESv2d.pdb )
		ADD_CUSTOM_COMMAND(	TARGET ${_PROJNAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E echo Copying EGL libraries...
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/libEGL.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/libEGLd.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/libEGLd.pdb $<TARGET_FILE_DIR:${_DIR_NAME}>/
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/libGLESv2.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/libGLESv2d.dll $<TARGET_FILE_DIR:${_DIR_NAME}>/
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Bin_DIR}/libGLESv2d.pdb $<TARGET_FILE_DIR:${_DIR_NAME}>/
		)
	ENDIF()

ENDFUNCTION()

IF( BW_IS_GENERIC_APP_TEST OR BW_IS_QT_TOOLS )
	BW_COPY_QT_DLLS( ${PROJECT_NAME} generic_app )
ENDIF()

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
