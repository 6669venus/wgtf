CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( core_control_system )

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
BW_ADD_LIBRARY( core_control_system ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_control_system INTERFACE
	core_reflection
	core_data_model
)

BW_PROJECT_CATEGORY( core_control_system "NGT Libs" )
