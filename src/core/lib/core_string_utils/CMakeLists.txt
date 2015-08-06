CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( core_string_utils )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( ALL_SRCS
	string_utils.hpp
	string_utils.cpp
	text_utilities.hpp
	text_utilities.cpp
)
SOURCE_GROUP( "Source" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS} 
)
BW_ADD_LIBRARY( core_string_utils ${BLOB_SRCS} )

BW_PROJECT_CATEGORY( core_string_utils "NGT Libs" )

