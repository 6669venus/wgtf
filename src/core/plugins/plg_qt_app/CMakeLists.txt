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
	ngt_core_common
	generic_plugin
	qt_common
	qt_script
	ui_framework
)

# Link QT5 modules
qt5_use_modules( ${PROJECT_NAME} Core Gui Widgets Quick )


FUNCTION( BW_COPY_FILES _PROJNAME _PREFIX _DIR _REQUIRED _FILES )
	FOREACH ( _FILE ${_FILES} )
		IF( EXISTS ${_FILE} )
			ADD_CUSTOM_COMMAND(	TARGET ${_PROJNAME} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${_PREFIX}>/${_DIR}
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_FILE} $<TARGET_FILE_DIR:${_PREFIX}>/${_DIR}
			)
		ELSEIF(${_REQUIRED})
			MESSAGE( FATAL_ERROR "File ${_FILE} is missing." )
		ENDIF()
	ENDFOREACH()
ENDFUNCTION()

# Copy the QT5 dlls
# Note: this should not be called from multiple plugins
#	because it may fail if executed concurrently
FUNCTION( BW_COPY_QT_DLLS _PROJNAME _DIR_NAME )

	ADD_CUSTOM_COMMAND(	TARGET ${_PROJNAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E echo Copying QT libraries...

		COMMAND ${CMAKE_COMMAND} -E copy_directory ${Qt5_DIR}/qml/QtQuick/Layouts/ $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick/Layouts/
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${Qt5_DIR}/qml/QtQuick/PrivateWidgets/ $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick/PrivateWidgets/
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${Qt5_DIR}/qml/QtQuick/Window.2/ $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick/Window.2/
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${Qt5_DIR}/qml/QtQuick/XmlListModel/ $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick/XmlListModel/
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${Qt5_DIR}/qml/QtQuick.2/ $<TARGET_FILE_DIR:${_DIR_NAME}>/QtQuick.2/
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${Qt5_DIR}/qml/QtGraphicalEffects/ $<TARGET_FILE_DIR:${_DIR_NAME}>/QtGraphicalEffects/
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${Qt5_DIR}/plugins/qmltooling/ $<TARGET_FILE_DIR:${_DIR_NAME}>/qmltooling/
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${Qt5Plugins_DIR}/platforms/ $<TARGET_FILE_DIR:${_DIR_NAME}>/platforms/
	)

	SET( QTQUICK_CONTROLS_FILES
		${Qt5_DIR}/qml/QtQuick/Controls/qtquickcontrolsplugin.dll
		${Qt5_DIR}/qml/QtQuick/Controls/qtquickcontrolsplugind.dll
		${Qt5_DIR}/qml/QtQuick/Controls/qmldir
	)

	SET( QTQUICK_CONTROLS_FILES_OPTIONAL
		${Qt5_DIR}/qml/QtQuick/Controls/qtquickcontrolsplugind.pdb
	)

	BW_COPY_FILES( ${_PROJNAME} ${_DIR_NAME} QtQuick/Controls TRUE "${QTQUICK_CONTROLS_FILES}" )
	BW_COPY_FILES( ${_PROJNAME} ${_DIR_NAME} QtQuick/Controls FALSE "${QTQUICK_CONTROLS_FILES_OPTIONAL}" )

	SET( QTQUICK_DIALOGS_FILES
		${Qt5_DIR}/qml/QtQuick/Dialogs/dialogplugin.dll
		${Qt5_DIR}/qml/QtQuick/Dialogs/dialogplugind.dll
		${Qt5_DIR}/qml/QtQuick/Dialogs/qmldir
	)

	SET( QTQUICK_DIALOGS_FILES_OPTIONAL
		${Qt5_DIR}/qml/QtQuick/Dialogs/dialogplugind.pdb
	)

	BW_COPY_FILES( ${_PROJNAME} ${_DIR_NAME} QtQuick/Dialogs TRUE "${QTQUICK_DIALOGS_FILES}" )
	BW_COPY_FILES( ${_PROJNAME} ${_DIR_NAME} QtQuick/Dialogs FALSE "${QTQUICK_DIALOGS_FILES_OPTIONAL}" )

	SET ( QT5_BIN_DLL_FILES
		${Qt5Bin_DIR}/Qt5Core.dll
		${Qt5Bin_DIR}/Qt5Cored.dll
		${Qt5Bin_DIR}/Qt5Gui.dll
		${Qt5Bin_DIR}/Qt5Guid.dll
		${Qt5Bin_DIR}/Qt5Network.dll
		${Qt5Bin_DIR}/Qt5Networkd.dll
		${Qt5Bin_DIR}/Qt5Qml.dll
		${Qt5Bin_DIR}/Qt5Qmld.dll
		${Qt5Bin_DIR}/Qt5Quick.dll
		${Qt5Bin_DIR}/Qt5Quickd.dll
		${Qt5Bin_DIR}/Qt5Widgets.dll
		${Qt5Bin_DIR}/Qt5Widgetsd.dll
		${Qt5Bin_DIR}/Qt5QuickWidgets.dll
		${Qt5Bin_DIR}/Qt5QuickWidgetsd.dll
		${Qt5Bin_DIR}/Qt5Xml.dll
		${Qt5Bin_DIR}/Qt5Xmld.dll
	)

	BW_COPY_FILES( ${_PROJNAME} ${_DIR_NAME} "" TRUE "${QT5_BIN_DLL_FILES}" )

	SET ( QT5_BIN_DLL_FILES_OPTIONAL
		${Qt5Bin_DIR}/Qt5Cored.pdb
		${Qt5Bin_DIR}/Qt5Guid.pdb
		${Qt5Bin_DIR}/Qt5Networkd.pdb
		${Qt5Bin_DIR}/Qt5Qmld.pdb
		${Qt5Bin_DIR}/Qt5Quickd.pdb
		${Qt5Bin_DIR}/Qt5Widgetsd.pdb
		${Qt5Bin_DIR}/Qt5QuickWidgetsd.pdb
		${Qt5Bin_DIR}/Qt5Xmld.pdb
	)

	BW_COPY_FILES( ${_PROJNAME} ${_DIR_NAME} "" FALSE "${QT5_BIN_DLL_FILES_OPTIONAL}" )

	FILE( GLOB QT5_ICU_FILES ${Qt5Bin_DIR}/icu*.dll )
	BW_COPY_FILES( ${_PROJNAME} ${_DIR_NAME} "" FALSE "${QT5_ICU_FILES}" )

	SET( QT5_ANGLE_FILES
		${Qt5Bin_DIR}/libEGL.dll
		${Qt5Bin_DIR}/libEGLd.dll
		${Qt5Bin_DIR}/libEGLd.pdb
		${Qt5Bin_DIR}/libGLESv2.dll
		${Qt5Bin_DIR}/libGLESv2d.dll
		${Qt5Bin_DIR}/libGLESv2d.pdb
	)
	BW_COPY_FILES( ${_PROJNAME} ${_DIR_NAME} "" FALSE "${QT5_ANGLE_FILES}" )

ENDFUNCTION()

# Copy the QT5 dylibs and frameworks on MAC OSX platform
# Note: this should not be called from multiple plugins
#	because it may fail if executed concurrently
FUNCTION( BW_COPY_QT_DYLIBS _PROJNAME _DIR_NAME )



ENDFUNCTION()

IF( BW_IS_GENERIC_APP_TEST OR BW_IS_QT_TOOLS )
	if (MACOSX)
		BW_COPY_QT_DYLIBS( ${PROJECT_NAME} generic_app )
	else (MACOSX)
		BW_COPY_QT_DLLS( ${PROJECT_NAME} generic_app )
	endif (MACOSX)
ENDIF()

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
