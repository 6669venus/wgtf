CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_perforce )
INCLUDE( WGToolsProject )

FIND_PACKAGE( Perforce "15.2" )

IF( NOT PERFORCE_FOUND )
	MESSAGE( STATUS "Disabled ${PROJECT_NAME}. Perforce not found in ${PERFORCE_DIR}." )
	# Use ADD_CUSTOM_TARGET because this target has no cpp files
	ADD_CUSTOM_TARGET( ${PROJECT_NAME} COMMENT "${PROJECT_NAME} disabled." )
	BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Disabled" )
	RETURN()
ENDIF()

INCLUDE_DIRECTORIES( ${WG_TOOLS_SOURCE_DIR}/core/interfaces )

SET( PERFORCE_DIR ${PERFORCE_DIR}$<$<CONFIG:Debug>:_debug> )
SET( CONFIG_SUFFIX $<$<CONFIG:Debug>:d>$<$<NOT:$<CONFIG:Debug>>:r> )
SET( LIB_SUFFIX ${PLATFORM_SUFFIX}${BITNESS_SUFFIX}${CONFIG_SUFFIX} )
SET( LIBEAY_PATH ${OPENSSL_DIR}/lib/libeay_${LIB_SUFFIX}.lib )
SET( SSLEAY_PATH ${OPENSSL_DIR}/lib/ssleay_${LIB_SUFFIX}.lib )
SET( PERFORCE_LIB_DIR ${PERFORCE_DIR}/Lib )

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

BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${BLOB_SRCS}
)

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_generic_plugin
	core_string_utils
	Ws2_32
	${PERFORCE_DIR}/lib/libclient.lib
	${PERFORCE_DIR}/lib/librpc.lib
	${PERFORCE_DIR}/lib/libsupp.lib
	${LIBEAY_PATH}
	${SSLEAY_PATH}
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
