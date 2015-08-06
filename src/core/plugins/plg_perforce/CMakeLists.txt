CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_perforce )

INCLUDE( BWStandardProject )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/core/interfaces )

IF( ${CMAKE_GENERATOR} MATCHES "Visual Studio 11( 2012)? Win64" )
	SET( PERFORCE_DIR ${CMAKE_CURRENT_LIST_DIR}/perforce/r15.1/bin.ntx64/vs2012_dyn$<$<CONFIG:Debug>:_debug> )
ELSEIF( ${CMAKE_GENERATOR} MATCHES "Visual Studio 11( 2012)?" )
	SET( PERFORCE_DIR ${CMAKE_CURRENT_LIST_DIR}/perforce/r15.1/bin.ntx86/vs2012_dyn$<$<CONFIG:Debug>:_debug> )
ELSEIF( ${CMAKE_GENERATOR} MATCHES "Visual Studio 12( 2013)? Win64" )
	SET( PERFORCE_DIR ${CMAKE_CURRENT_LIST_DIR}/perforce/r15.1/bin.ntx64/vs2013_dyn$<$<CONFIG:Debug>:_debug> )
ELSEIF( ${CMAKE_GENERATOR} MATCHES "Visual Studio 12( 2013)?" )
	SET( PERFORCE_DIR ${CMAKE_CURRENT_LIST_DIR}/perforce/r15.1/bin.ntx86/vs2013_dyn$<$<CONFIG:Debug>:_debug> )
ENDIF()

SET( OPENSSL_DIR ${CMAKE_CURRENT_LIST_DIR}/OpenSSL )

IF( WIN32 )
	SET( PLATFORM_SUFFIX w )
ELSE()
	MESSAGE( FATAL_ERROR "Currently only windows libraries are available" )
ENDIF()

SET( CONFIG_SUFFIX $<$<CONFIG:Debug>:d>$<$<NOT:$<CONFIG:Debug>>:r> )
IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	SET( BITNESS_SUFFIX 64)
ENDIF()
SET( LIB_SUFFIX ${PLATFORM_SUFFIX}${BITNESS_SUFFIX}${CONFIG_SUFFIX} )
SET( LIBEAY_PATH ${OPENSSL_DIR}/lib/libeay_${LIB_SUFFIX}.lib )
SET( SSLEAY_PATH ${OPENSSL_DIR}/lib/ssleay_${LIB_SUFFIX}.lib )

INCLUDE_DIRECTORIES( ${PERFORCE_DIR}/include )

SET( INTERFACES
	../../interfaces/version_control/i_version_control.hpp
	../../interfaces/version_control/i_depot_view.hpp
	../../interfaces/version_control/i_result.hpp
)
SOURCE_GROUP( "Interfaces" FILES ${INTERFACES} )

SET( ALL_SRCS
	perforce_depot_view.cpp
	perforce_depot_view.hpp
	perforce_result.cpp
	perforce_result.hpp
	perforce_version_control.hpp
	perforce_version_control.cpp
	plg_perforce.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
	${INTERFACES}
)

BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${BLOB_SRCS}
)

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_generic_plugin
	Ws2_32
	${PERFORCE_DIR}/lib/libclient.lib
	${PERFORCE_DIR}/lib/librpc.lib
	${PERFORCE_DIR}/lib/libsupp.lib
	${LIBEAY_PATH}
	${SSLEAY_PATH}
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )