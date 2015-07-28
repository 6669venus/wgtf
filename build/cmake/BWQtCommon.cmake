#
# Common directory structure for Qt 5.1.1 Projects built with CMake 2.8.11 (or later)
#

CMAKE_MINIMUM_REQUIRED( VERSION 2.8.11 )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Setup Qt5 Build Paths

SET (Qt5_DIR "${BW_SOURCE_DIR}/core/third_party/Qt/${QT_VERSION}" )

IF( NOT EXISTS "${Qt5_DIR}/" )
	MESSAGE( FATAL_ERROR "Please clone Qt third party repository into ${Qt5_DIR} for Qt ${QT_VERSION} build." )
ENDIF()

IF ( CMAKE_LINKER MATCHES "Visual Studio 11" )
	SET( Qt5_DIR "${Qt5_DIR}/msvc2012" )
ELSEIF( CMAKE_LINKER MATCHES "Visual Studio 12" )
	SET( Qt5_DIR "${Qt5_DIR}/msvc2013" )
ELSE()
	SET( Qt5_DIR "${Qt5_DIR}/___unsupported___" )
ENDIF()

IF (BW_ARCH_64)
	SET( Qt5_DIR "${Qt5_DIR}_64" )
ENDIF()

IF( NOT EXISTS "${Qt5_DIR}/" AND EXISTS "${Qt5_DIR}_opengl/" )
	SET( Qt5_DIR "${Qt5_DIR}_opengl" )
ENDIF()

IF( NOT EXISTS "${Qt5_DIR}/" )
	MESSAGE( FATAL_ERROR "Qt build for \"Qt ${QT_VERSION}\" with \"${CMAKE_GENERATOR}\" is not supported, required Qt directory \"${Qt5_DIR}\" does not exists." )
ENDIF()

IF( CMAKE_BUILD_TYPE MATCHES DEBUG )
	ADD_DEFINITIONS( "-DQT_QML_DEBUG_NO_WARNING" )
	SET( QT_QML_DEBUG TRUE )
ENDIF()

SET( CMAKE_PREFIX_PATH ${Qt5_DIR} CMAKE_PREFIX_PATH )

# Setup Post-buildQt5  paths
SET( Qt5Bin_DIR "${Qt5_DIR}/bin" )
SET( Qt5Plugins_DIR "${Qt5_DIR}/plugins" )

# Include the required Qt5 Packages
find_package( Qt5Core REQUIRED )
find_package( Qt5Widgets REQUIRED )
find_package( Qt5Gui REQUIRED )
find_package( Qt5Qml REQUIRED )

SET(DEPLOY_QT_COMMAND "${CMAKE_CURRENT_LIST_DIR}/../deployqt.bat")
FUNCTION( BW_DEPLOY_QT _TARGET )
	ADD_CUSTOM_COMMAND(	TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND "${DEPLOY_QT_COMMAND}" ${Qt5Bin_DIR}
			$<$<NOT:$<CONFIG:Debug>>:--release>$<$<CONFIG:Debug>:--debug>
			--dir "$<TARGET_FILE_DIR:${_TARGET}>"
			--qmldir "${CMAKE_CURRENT_SOURCE_DIR}" "$<TARGET_FILE:${PROJECT_NAME}>"
	)
ENDFUNCTION()
