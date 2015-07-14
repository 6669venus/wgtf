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

IF (BW_ARCH_64)
	IF ( CMAKE_LINKER MATCHES "Visual Studio 11" )
		SET( Qt5_DIR "${Qt5_DIR}/msvc2012_64" )
	ELSEIF( CMAKE_LINKER MATCHES "Visual Studio 12" )
		SET( Qt5_DIR "${Qt5_DIR}/msvc2013_64" )
	ELSE()
		SET( Qt5_DIR "${Qt5_DIR}/___unsupported___" )
	ENDIF()
ELSEIF( ${QT_VERSION} STREQUAL "5.3.2" )
	# Special case for Qt 5.3.2
	SET( Qt5_DIR "${Qt5_DIR}/msvc2012_opengl" )
ELSE()
	SET( Qt5_DIR "${Qt5_DIR}/___unsupported___" )
ENDIF()

IF( NOT EXISTS "${Qt5_DIR}/" )
	MESSAGE( FATAL_ERROR "Qt build for \"Qt ${QT_VERSION}\" with \"${CMAKE_GENERATOR}\" is not supported." )
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

