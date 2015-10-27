#
# Common directory structure for Qt 5 Projects built with CMake 3.1.1 (or later)
#

CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )

INCLUDE( BWPlatformOptions )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Setup Qt5 Build Paths

IF (NOT Qt5_DIR)
	SET (Qt5_DIR "${WG_TOOLS_SOURCE_DIR}/core/third_party/Qt/${QT_VERSION}" )

	IF ( BW_PLATFORM_WINDOWS )
		IF ( CMAKE_GENERATOR_TOOLSET STREQUAL "v110_xp" )
			SET( Qt5_DIR "${Qt5_DIR}/msvc2012" )
		ELSEIF( CMAKE_GENERATOR_TOOLSET STREQUAL "v120_xp" )
			SET( Qt5_DIR "${Qt5_DIR}/msvc2013" )
		ELSE()
			MESSAGE( FATAL_ERROR "Cannot find Qt binaries built with the ${CMAKE_GENERATOR_TOOLSET} toolset." )
		ENDIF()
	ELSEIF( BW_PLATFORM_MAC )
		SET( Qt5_DIR "${Qt5_DIR}/clang" )
	ELSE()
		SET( Qt5_DIR "${Qt5_DIR}/___unsupported___" )
	ENDIF()

	IF (BW_ARCH_64)
		SET( Qt5_DIR "${Qt5_DIR}_64" )
	ENDIF()

	IF( NOT EXISTS "${Qt5_DIR}/" AND EXISTS "${Qt5_DIR}_opengl/" )
		SET( Qt5_DIR "${Qt5_DIR}_opengl" )
	ENDIF()
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
find_package( Qt5Gui REQUIRED )
find_package( Qt5Qml REQUIRED )
find_package( Qt5Quick REQUIRED )
find_package( Qt5QuickWidgets REQUIRED )
find_package( Qt5UiTools REQUIRED )
find_package( Qt5Widgets REQUIRED )
find_package( Qt5Xml REQUIRED )

IF(BW_PLATFORM_WINDOWS)
	SET(DEPLOY_QT_COMMAND "${CMAKE_CURRENT_LIST_DIR}/../deployqt.bat")
ELSEIF(BW_PLATFORM_MAC)
	SET(DEPLOY_QT_COMMAND "${CMAKE_CURRENT_LIST_DIR}/../deployqt.sh")
ENDIF()

FUNCTION( BW_DEPLOY_QT _TARGET )
	IF(NOT TARGET ${_TARGET})
		MESSAGE("Ignoring BW_DEPLOY_QT for non-existing Target '${_TARGET}'")
	    RETURN()
	ENDIF()

	IF(BW_PLATFORM_WINDOWS)
		ADD_CUSTOM_COMMAND(	TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND "${DEPLOY_QT_COMMAND}" ${Qt5Bin_DIR} $<$<CONFIG:Hybrid>:--release>
				--dir "$<TARGET_FILE_DIR:${_TARGET}>"
				--qmldir "${CMAKE_CURRENT_SOURCE_DIR}" "$<TARGET_FILE:${PROJECT_NAME}>"
		)
	ELSEIF(BW_PLATFORM_MAC)
		ADD_CUSTOM_COMMAND(	TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND "${DEPLOY_QT_COMMAND}" ${Qt5Bin_DIR} $<TARGET_FILE_DIR:${_TARGET}>/../..
				-always-overwrite
				-qmldir="${CMAKE_CURRENT_SOURCE_DIR}"
		)
	ELSE()
	    MESSAGE( FATAL_ERROR "BW_DEPLOY_QT(): Unsupported platform!" )
	ENDIF()
ENDFUNCTION()

MACRO(BW_GLOB_RESOURCE_FILES)
    FILE(GLOB_RECURSE QML_SRCS *.qml qmldir)
    SOURCE_GROUP("QML Files" FILES ${QML_SRCS})

    FILE(GLOB_RECURSE JS_SRCS *.js)
    SOURCE_GROUP("JS Files" FILES ${JS_SRCS})

    FILE(GLOB_RECURSE UI_SRCS *.ui)
    SOURCE_GROUP("UI Files" FILES ${UI_SRCS})

    FILE(GLOB_RECURSE ICON_SRCS *.png *.bmp)
    SOURCE_GROUP("Icon Files" FILES ${ICON_SRCS})

    SET( ALL_SRCS ${ALL_SRCS} ${QML_SRCS} ${JS_SRCS} ${UI_FILES})
ENDMACRO()

IF( BW_PLATFORM_WINDOWS )
    SET( QRC_GENERATOR ${CMAKE_CURRENT_LIST_DIR}/../qrc_generator.bat )
ELSE()
    SET( QRC_GENERATOR ${CMAKE_CURRENT_LIST_DIR}/../qrc_generator.sh )
ENDIF()

MACRO(BW_QT_ADD_QRC _PREFIX _DIR)
    IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_DIR}" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${_DIR}")
        SET(_RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${_DIR}")
    ELSEIF(EXISTS "${_DIR}" AND IS_DIRECTORY "${_DIR}")
        SET(_RESOURCE_DIR ${_DIR})
    ENDIF()

    EXECUTE_PROCESS(
        COMMAND ${QRC_GENERATOR} ${CMAKE_CURRENT_BINARY_DIR}/${_PREFIX}.qrc ${_PREFIX} ${_RESOURCE_DIR}
    )
    QT5_ADD_RESOURCES(COMPILED_RESOURCES ${CMAKE_CURRENT_BINARY_DIR}/${_PREFIX}.qrc)
    SET(RESOURCE_FILES ${RESOURCE_FILES} ${CMAKE_CURRENT_BINARY_DIR}/${_PREFIX}.qrc)
    SOURCE_GROUP( "Resource Files" FILES ${RESOURCE_FILES} )
    SOURCE_GROUP( "Compiled Resources" FILES ${COMPILED_RESOURCES} )
ENDMACRO()
