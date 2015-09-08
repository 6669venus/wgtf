CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( core_serialization )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( SERIALIZER_SRCS
	serializer/i_serializer.hpp
	serializer/i_serialization_manager.hpp
	serializer/serialization_manager.cpp
	serializer/serialization_manager.hpp
)
SOURCE_GROUP( "Serializer" FILES ${SERIALIZER_SRCS} )

SET( ALL_SRCS
	basic_stream.cpp
	basic_stream.hpp
	binary_stream.cpp
	binary_stream.hpp
	datastreambuf.hpp
	file_data_stream.cpp
	file_data_stream.hpp
	file_system.hpp
	fixed_memory_stream.hpp
	fixed_memory_stream.cpp
	i_datastream.cpp
	i_datastream.hpp
	resizing_memory_stream.hpp
	resizing_memory_stream.cpp
	std_data_stream.hpp
	text_stream.cpp
	text_stream.hpp
	text_stream_manip.cpp
	text_stream_manip.hpp
)

BW_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )

IF( BW_PLATFORM_WINDOWS )
	INCLUDE( "CMakeLists.windows.txt" )
ELSEIF( BW_PLATFORM_MAC )
	INCLUDE( "CMakeLists.mac.txt" )
ENDIF()

SOURCE_GROUP( "Source" FILES ${BLOB_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${SERIALIZER_SRCS}
	${BLOB_SRCS}
)

BW_ADD_LIBRARY( core_serialization ${BLOB_SRCS} )


BW_TARGET_LINK_LIBRARIES( core_serialization INTERFACE
	core_variant
)

BW_PROJECT_CATEGORY( core_serialization "NGT Libs" )
