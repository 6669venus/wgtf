CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( copy_paste )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )


SET( ALL_SRCS
	copy_paste_manager.cpp
	copy_paste_manager.hpp
	i_copy_paste_manager.hpp
	i_copyable_object.hpp
)
SOURCE_GROUP( "Source" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( copy_paste ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( copy_paste INTERFACE
	serialization
	command_system
	variant
)

BW_PROJECT_CATEGORY( copy_paste "NGT Libs" )

