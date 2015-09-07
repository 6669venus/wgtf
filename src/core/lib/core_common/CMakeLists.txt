CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( core_common )

IF( BW_PLATFORM_WINDOWS )
	INCLUDE( "CMakeLists.windows.txt" )
ELSE()
	INCLUDE( "CMakeLists.unix.txt" )
ENDIF()

SET( ALL_SRCS
	CMakeLists.unix.txt
	CMakeLists.windows.txt
	platform_dbg.hpp
	platform_dbg.cpp
	platform_dll.hpp
	platform_dll.cpp
	platform_env.hpp
	platform_env.cpp
	platform_path.hpp
	platform_path.cpp
	platform_std.hpp
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
