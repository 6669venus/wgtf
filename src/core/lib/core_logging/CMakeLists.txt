CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )

INCLUDE( BWStandardProject )
INCLUDE( BWStandardLibrary )
PROJECT( core_logging )

SET( ALL_SRCS
	logging.cpp
	logging.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS 
	${ALL_SRCS}
)
BW_ADD_LIBRARY( core_logging ${BLOB_SRCS} )


BW_PROJECT_CATEGORY( core_logging "Editor Shared" )

