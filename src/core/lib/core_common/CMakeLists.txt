CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( core_common )

IF( WIN32 )
	INCLUDE( "CMakeLists.windows.txt" )
ELSE()
	INCLUDE( "CMakeLists.unix.txt" )
ENDIF()

SET( ALL_SRCS
	environment.hpp
	shared_library.cpp
	shared_library.hpp
	signal.hpp
	wg_condition_variable.cpp
	wg_condition_variable.hpp
	${PLATFORM_SRCS}
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( core_common ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_common PRIVATE
	${PLATFORM_LIBRARIES}
)

BW_PROJECT_CATEGORY( core_common "NGT Libs" )
