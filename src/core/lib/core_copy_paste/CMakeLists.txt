CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( core_copy_paste )

INCLUDE( BWStandardProject )
INCLUDE( BWStandardLibrary )


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
BW_ADD_LIBRARY( core_copy_paste ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_copy_paste INTERFACE
	core_serialization
	core_command_system
	core_variant
)

BW_PROJECT_CATEGORY( core_copy_paste "NGT Libs" )

