CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( bw_qt_common )

INCLUDE( WGToolsProject )
INCLUDE( WGToolsLibrary )
INCLUDE( BWQtLibrary )


SET( ALL_SRCS
	app/i_editor_app.hpp
	app/i_editor_app.cpp
	cursor/cursor.hpp
	cursor/cursor.cpp
	cursor/wait_cursor.hpp
	cursor/wait_cursor.cpp
	dialogs/file_dialog.hpp
	dialogs/file_dialog.cpp
	dialogs/folder_guard.hpp
	dialogs/message_box.hpp
	dialogs/message_box.cpp
	mouse/mouse_state.hpp
	mouse/mouse_state.cpp
	view/qt_gl_view.hpp
	view/qt_gl_view.cpp
	menu/menu_helper.hpp
	menu/menu_helper.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )

BW_ADD_LIBRARY( bw_qt_common ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( bw_qt_common INTERFACE )

# Link QT5 modules
qt5_use_modules( bw_qt_common LINK_PRIVATE Core Gui Widgets Quick UiTools )

BW_PROJECT_CATEGORY( bw_qt_common "Editor Shared" )
