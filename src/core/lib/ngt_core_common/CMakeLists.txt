CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( ngt_core_common )

SET( ALL_SRCS
	wg_condition_variable.cpp
	wg_condition_variable.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( ngt_core_common ${BLOB_SRCS} )

BW_PROJECT_CATEGORY( ngt_core_common "NGT Libs" )
