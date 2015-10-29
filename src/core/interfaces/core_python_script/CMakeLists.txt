CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( core_python_script )

INCLUDE( WGToolsCoreProject )


SET( V0_INTERFACES
	v0/i_module_v0.hpp
	v0/i_scripting_engine_v0.hpp
)
SOURCE_GROUP( "v0" FILES ${V0_INTERFACES} )


SET( ALL_SRCS
	i_module.hpp
	i_scripting_engine.hpp
	python_meta_type.cpp
	python_meta_type.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( ${PROJECT_NAME}
	${V0_INTERFACES}
	${BLOB_SRCS} )
 
BW_PROJECT_CATEGORY( core_python_script "NGT Libs" )

