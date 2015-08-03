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
	file_data_stream.cpp
	file_data_stream.hpp
	file_system.hpp
	memory_stream_wrapper.hpp
	memory_stream_wrapper.cpp
	resizing_memory_stream.hpp
	resizing_memory_stream.cpp
	text_stream.hpp
	text_stream.cpp
	datastream_adapter.h
)

BW_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )

IF( BW_PLATFORM_WINDOWS )
	INCLUDE( "CMakeLists.windows.txt" )
ENDIF()

SOURCE_GROUP( "Source" FILES ${BLOB_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${SERIALIZER_SRCS}
	${BLOB_SRCS}
)
	
BW_ADD_LIBRARY( serialization ${BLOB_SRCS} )


BW_TARGET_LINK_LIBRARIES( serialization INTERFACE
	variant
)

BW_PROJECT_CATEGORY( serialization "NGT Libs" )