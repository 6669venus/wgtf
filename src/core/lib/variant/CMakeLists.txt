CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( variant )

INCLUDE( BWStandardProject )
INCLUDE( BWStandardLibrary )

SET( INTERFACES_SRC
	interfaces/i_meta_type_manager.hpp
)
SOURCE_GROUP( "interfaces" FILES ${INTERFACES_SRC} )

SET( ALL_SRCS
	collection.cpp
	collection.hpp
	container_helper.hpp
	container_helper.cpp
	default_meta_type_manager.hpp
	default_meta_type_manager.cpp
	meta_type.hpp
	type_id.hpp
	type_id.cpp
	variant.cpp
	variant.hpp
)
SOURCE_GROUP( "Source" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${INTERFACES_SRC}
	${ALL_SRCS}
)
BW_ADD_LIBRARY( variant ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( variant INTERFACE
	wg_types
)

BW_PROJECT_CATEGORY( variant "NGT Libs" )

