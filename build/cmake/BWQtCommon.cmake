#
# Common directory structure for Qt 5.1.1 Projects built with CMake 2.8.11 (or later)
#

CMAKE_MINIMUM_REQUIRED( VERSION 2.8.11 )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Setup Qt5 Build Paths
IF( ${BW_PLATFORM} STREQUAL "win64" )
	IF( ${QT_VERSION} STREQUAL "5.3.1" )
		SET( Qt5_DIR "${BW_SOURCE_DIR}/core/third_party/Qt/5.3.1/msvc2012_64" )
	ELSEIF( ${QT_VERSION} STREQUAL "5.4.2" )
		SET( Qt5_DIR "${BW_SOURCE_DIR}/core/third_party/Qt/5.4.2/msvc2012_64" )
	ELSE()
		MESSAGE( FATAL_ERROR "Qt build for ${QT_VERSION} is not supported." )
	ENDIF()
ELSE()
	IF( ${QT_VERSION} STREQUAL "5.3.2" )
		SET( Qt5_DIR "${BW_SOURCE_DIR}/core/third_party/Qt/5.3.2/msvc2012_opengl" )
	ELSE()
		MESSAGE( FATAL_ERROR "Qt build for ${QT_VERSION} is not supported." )
	ENDIF()
ENDIF()

SET( CMAKE_PREFIX_PATH ${Qt5_DIR} CMAKE_PREFIX_PATH )

# Setup Post-buildQt5  paths
SET( Qt5Bin_DIR "${Qt5_DIR}/bin" )
SET( Qt5Plugins_DIR "${Qt5_DIR}/plugins" )

# Include the required Qt5 Packages
find_package( Qt5Core REQUIRED )
find_package( Qt5Widgets REQUIRED )
find_package( Qt5Gui REQUIRED )

