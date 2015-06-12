CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

INCLUDE( BWStandardProject )
INCLUDE( BWStandardLibrary )

SET( SHARED_SOURCE_FILES_SRCS
	gui/editor_main_frame.cpp
	gui/editor_main_frame.hpp
	gui/i_main_frame.hpp
	gui/folder_guard.hpp
	gui/folder_guard.cpp
	pages/gui_tab_content.hpp
	view/gl_view.hpp
	view/gl_view.cpp
)
SOURCE_GROUP( "Shared_Source_Files" FILES ${SHARED_SOURCE_FILES_SRCS} )

SET( ALL_SRCS
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

SET( SHARED_ALL_SRCS
	${ALL_SRCS}
	${SHARED_SOURCE_FILES_SRCS}
)

IF( BW_IS_QT_TOOLS OR BW_IS_GENERIC_APP_TEST )
	ADD_DEFINITIONS(-DQT_TOOLS)
	BW_BLOB_SOURCES( BLOB_SRCS
		${SHARED_ALL_SRCS}
	)
	BW_ADD_LIBRARY( editor_shared ${BLOB_SRCS} )
ELSE()
	INCLUDE( "CMakeLists.mfc.txt" )
ENDIF()

BW_TARGET_LINK_LIBRARIES( editor_shared INTERFACE
)
BW_PROJECT_CATEGORY( editor_shared "Editor Shared" )
