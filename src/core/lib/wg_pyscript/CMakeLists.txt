CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( wg_pyscript )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

FIND_PACKAGE( Python "2.7.10" )
IF( NOT PYTHON_FOUND )
	# Use ADD_CUSTOM_TARGET because this target has no cpp files
	ADD_CUSTOM_TARGET( ${PROJECT_NAME} COMMENT "${PROJECT_NAME} disabled." )
	BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Disabled" )
	RETURN()
ENDIF()
INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_DIRS} )

SET( ALL_SRCS
	pch.cpp
	pch.hpp
	py_script_args.ipp
	py_script_dict.ipp
	py_script_list.ipp
	py_script_module.ipp
	py_script_object.cpp
	py_script_object.hpp
	py_script_sequence.ipp
	py_script_tuple.ipp
	pyobject_pointer.hpp
	type_converter.cpp
	type_converter.hpp
)
SOURCE_GROUP( "Source" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS} 
)
BW_ADD_LIBRARY( wg_pyscript ${BLOB_SRCS} )

BW_PRECOMPILED_HEADER( wg_pyscript pch.hpp )
BW_PROJECT_CATEGORY( wg_pyscript "Wargaming Libs" )

