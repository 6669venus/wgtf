#include "editor_main_frame.hpp"
#include "bw_qt_common/view/gl_view.hpp"


EditorMainFrame::EditorMainFrame()
	: editorApp_( nullptr )
	, glView_( nullptr )
{

}


BW::Vector2 EditorMainFrame::currentCursorPosition() const
{
	return BW::Vector2(0, 0);
}


BW::Vector3 EditorMainFrame::getWorldRay( int x, int y ) const
{
	return BW::Vector3(0, 0, 0);
}


bool EditorMainFrame::cursorOverGraphicsWnd() const
{
	return glView_->underMouse();
}


void EditorMainFrame::setEditorApp( IEditorApp* pEditorApp )
{
	editorApp_ = pEditorApp;
}


IEditorApp* EditorMainFrame::getEditorApp()
{
	return editorApp_;
}


void EditorMainFrame::setEditorView( GLView* glView )
{
	glView_ = glView;
}


GLView* EditorMainFrame::getEditorView()
{
	return glView_;
}


const GLView* EditorMainFrame::getEditorView() const
{
	return glView_;
}


BW::GUI::IMenuHelper* EditorMainFrame::getMenuHelper()
{
	return menuHelper_.get();
}


void EditorMainFrame::setMenuHelper( BW::GUI::IMenuHelper* menuHelper )
{
	menuHelper_.reset( menuHelper );
}


void EditorMainFrame::setMessageText( const wchar_t * pText )
{
	
}


void EditorMainFrame::setStatusText( unsigned int id, const wchar_t * text )
{
	
}