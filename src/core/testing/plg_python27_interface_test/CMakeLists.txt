CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( plg_python27_interface_test )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGPython27Project )

IF( NOT PYTHON_FOUND )
	RETURN()
ENDIF()


SET( PYTHON_SRCS
	scripts/python27_test/__init__.py
)
SOURCE_GROUP( "Python" FILES ${PYTHON_SRCS} )


SET( ALL_SRCS
	main_application.cpp
	main_application.hpp
	pch.cpp
	pch.hpp
	plg_python27_interface_test.cpp
	python27_interface_test.cpp
	python27_interface_test.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${BLOB_SRCS}
	${PYTHON_SRCS}
)
 
BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_generic_plugin
	core_reflection
	core_unit_test
)

BW_PRECOMPILED_HEADER( ${PROJECT_NAME} pch.hpp )
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )

