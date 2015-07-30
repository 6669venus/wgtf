CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( control_system )

INCLUDE( BWStandardProject )
INCLUDE( BWStandardLibrary )

SET( ALL_SRCS
	control_manager.hpp
	control_manager.cpp
)
SOURCE_GROUP( "Source" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS 
	${ALL_SRCS} 
)
BW_ADD_LIBRARY( control_system ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( control_system INTERFACE
	reflection
	data_model
)

BW_PROJECT_CATEGORY( control_system "NGT Libs" )
