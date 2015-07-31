CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( wg_types )

INCLUDE( NGTStandardProject )
INCLUDE( NGTStandardLibrary )

SET( ALL_SRCS
	binary_block.hpp
	binary_block.cpp
	hash_utilities.hpp
	hash_utilities.cpp
	hashed_string_ref.hpp
	hashed_string_ref.cpp
	event.hpp
	string_ref.hpp
	string_ref.cpp
)
SOURCE_GROUP( "Source" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS} 
)
BW_ADD_LIBRARY( wg_types ${BLOB_SRCS} )

BW_PROJECT_CATEGORY( wg_types "Wargaming Libs" )

