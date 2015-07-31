CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( string_utils )

INCLUDE( NGTStandardProject )
INCLUDE( NGTStandardLibrary )

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
BW_ADD_LIBRARY( string_utils ${BLOB_SRCS} )

BW_PROJECT_CATEGORY( string_utils "NGT Libs" )

