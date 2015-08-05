CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )


SET( ALL_SRCS
	logging.cpp
	logging.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS 
	${ALL_SRCS}
)
BW_ADD_LIBRARY( logging ${BLOB_SRCS} )


BW_PROJECT_CATEGORY( logging "Editor Shared" )

