CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( ngt_core_common )

IF( WIN32 )
	INCLUDE( "CMakeLists.windows.txt" )
ELSE()
	INCLUDE( "CMakeLists.unix.txt" )
ENDIF()

SET( ALL_SRCS
	shared_library.cpp
	shared_library.hpp
	wg_condition_variable.cpp
	wg_condition_variable.hpp
	${PLATFORM_SRCS}
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( ngt_core_common ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( ngt_core_common
	${PLATFORM_LIBRARIES}
)

BW_PROJECT_CATEGORY( ngt_core_common "NGT Libs" )
