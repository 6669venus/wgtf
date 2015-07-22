CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( test_plg_wgs_core )

INCLUDE( BWStandardProject )

# Setup Visual Studio Unit Test Include Paths
IF( ${CMAKE_GENERATOR} MATCHES "Visual Studio 11( 2012)?" )
	SET(VCINSTALLDIR $ENV{VS110COMNTOOLS}/../../VC/)
ELSEIF( ${CMAKE_GENERATOR} MATCHES "Visual Studio 12( 2013)?" )
	SET(VCINSTALLDIR $ENV{VS120COMNTOOLS}/../../VC/)
ENDIF()
INCLUDE_DIRECTORIES( ${VCINSTALLDIR}UnitTest/include )
LINK_DIRECTORIES( "${VCINSTALLDIR}UnitTest/lib/" )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

ADD_DEFINITIONS( 
	-DCSTDMF_IMPORT
)

# Find includes in corresponding build directories
SET( CMAKE_INCLUDE_CURRENT_DIR ON )

SET( ALL_SRCS
	stdafx.cpp
	stdafx.h
	test_plg_wgs_core.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( test_plg_wgs_core SHARED ${BLOB_SRCS} )
BW_PRECOMPILED_HEADER( test_plg_wgs_core stdafx.h )

BW_TARGET_LINK_LIBRARIES( test_plg_wgs_core PRIVATE
	generic_plugin_manager
	Microsoft.VisualStudio.TestTools.CppUnitTestFramework
)

BW_SET_BINARY_DIR( test_plg_wgs_core "${BW_GAME_DIR}/${TARGET_PLUGIN_APP_FOLDER_NAME}/${BW_PLATFORM}" )
BW_PROJECT_CATEGORY( test_plg_wgs_core "Unit Tests" )

ADD_DEPENDENCIES( test_plg_wgs_core plg_wgs_core )