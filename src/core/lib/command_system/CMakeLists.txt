CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( command_system )

INCLUDE( BWStandardProject )
INCLUDE( BWStandardLibrary )

SET( METADATA_SRCS
	metadata/compound_command.mpp
	metadata/command_instance.mpp
	metadata/macro_object.mpp
)
SOURCE_GROUP( "MetaData" FILES ${METADATA_SRCS} )

SET( ALL_SRCS
	i_command_event_listener.hpp
	compound_command.hpp 
	compound_command.cpp
	command_instance.hpp 
	command_instance.cpp
	command_manager.hpp 
	command_manager.cpp 
	command_system.hpp 
	command_system.cpp 
	command_system_provider.hpp 
	scoped_disable_command_recording.cpp
	scoped_disable_command_recording.hpp
	reflected_command.hpp 
	reflected_command.cpp
	macro_object.hpp
	macro_object.cpp
)
SOURCE_GROUP( "Source" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS 
	${ALL_SRCS} 
	${METADATA_SRCS}  
)
BW_ADD_LIBRARY( command_system ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( command_system INTERFACE
	ngt_core_common
	reflection_core
	serialization
	data_model
	logging
)

BW_PROJECT_CATEGORY( command_system "NGT Libs" )
