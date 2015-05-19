CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( ui_framework)

INCLUDE( BWStandardProject )
INCLUDE( BWStandardLibrary )

SET( ALL_SRCS
	func_action.cpp
	func_action.hpp
	generic_component_provider.hpp
	i_action.hpp
	i_component.hpp
	i_component_provider.hpp
	i_ui_application.hpp
	i_ui_framework.hpp
	i_view.hpp
	i_window.hpp
	layout_hint.cpp
	layout_hint.hpp
	simple_component_provider.hpp
)
SOURCE_GROUP( "Source" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( ui_framework ${BLOB_SRCS} )

BW_PROJECT_CATEGORY( ui_framework "NGT Libs" )

