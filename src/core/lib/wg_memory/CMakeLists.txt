CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( wg_memory )

INCLUDE( WGToolsProject )
INCLUDE( WGToolsLibrary )

SET( ALL_SRCS
	allocator.hpp
	allocator.cpp
	memory_overrides.hpp
)
SOURCE_GROUP( "Source" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS} 
)
BW_ADD_LIBRARY( wg_memory ${BLOB_SRCS} )

BW_PROJECT_CATEGORY( wg_memory "Wargaming Libs" )

