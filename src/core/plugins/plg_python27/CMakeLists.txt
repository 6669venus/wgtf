CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( plg_python27 )

INCLUDE( BWMacros )
INCLUDE( WGPython27Project )

SET( PYSCRIPT_SRCS
	pyscript/py_script_args.ipp
	pyscript/py_script_dict.ipp
	pyscript/py_script_list.ipp
	pyscript/py_script_module.ipp
	pyscript/py_script_object.cpp
	pyscript/py_script_object.hpp
	pyscript/py_script_sequence.ipp
	pyscript/py_script_tuple.ipp
	pyscript/type_converter.cpp
	pyscript/type_converter.hpp
)
SOURCE_GROUP( "pyscript" FILES ${PYSCRIPT_SRCS} )

SET( ALL_SRCS
	pch.cpp
	pch.hpp
	plg_python27.cpp
	interpreter.cpp
	interpreter.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
	${PYSCRIPT_SRCS}
)
BW_ADD_TOOL_PLUGIN( plg_python27 ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED
	${BLOB_SRCS}
)
 
BW_TARGET_LINK_LIBRARIES( plg_python27 PRIVATE
	core_generic_plugin
	libpython-shared )

BW_PRECOMPILED_HEADER( plg_python27 pch.hpp )
BW_PROJECT_CATEGORY( plg_python27 "Plugins" )

