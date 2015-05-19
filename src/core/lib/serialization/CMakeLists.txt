CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( serialization )

INCLUDE( BWStandardProject )
INCLUDE( BWStandardLibrary )

SET( SERIALIZER_SRCS
	serializer/i_serializer.hpp
	serializer/i_serialization_manager.hpp
	serializer/serialization_manager.cpp
	serializer/serialization_manager.hpp
)
SOURCE_GROUP( "Serializer" FILES ${SERIALIZER_SRCS} )

SET( ALL_SRCS
	i_datastream.hpp
	memory_stream_wrapper.hpp
	memory_stream_wrapper.cpp
	resizing_memory_stream.hpp
	resizing_memory_stream.cpp
	datastream_adapter.h
)
SOURCE_GROUP( "Source" FILES ${ALL_SRCS} )

BW_ADD_LIBRARY( serialization
	${SERIALIZER_SRCS}
	${ALL_SRCS}
)

BW_TARGET_LINK_LIBRARIES( serialization INTERFACE
	variant
)

BW_PROJECT_CATEGORY( serialization "NGT Libs" )