CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( serialization_unit_test )

INCLUDE( NGTStandardProject )

SET( BW_USE_CSTDMF_DLL ON )
SET( BW_USE_CSTDMF_IN_DLL ON )
ADD_DEFINITIONS(
	-DCSTDMF_IMPORT
)

SET( ALL_SRCS
	main.cpp
	pch.cpp
	pch.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )

IF( BW_PLATFORM_WINDOWS )
	INCLUDE( "CMakeLists.windows.txt" )
ENDIF()

BW_ADD_EXECUTABLE( serialization_unit_test ${BLOB_SRCS} )
BW_TARGET_LINK_LIBRARIES( serialization_unit_test PRIVATE
	serialization
	unit_test_lib
)

BW_ADD_TOOL_TEST( serialization_unit_test )

BW_PRECOMPILED_HEADER( serialization_unit_test pch.hpp )
BW_PROJECT_CATEGORY( serialization_unit_test "Unit Tests" )
