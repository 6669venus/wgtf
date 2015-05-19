#ifndef EDITOR_MAIN_FRAME_HPP
#define EDITOR_MAIN_FRAME_HPP

#include "editor_shared/interfaces/i_bw_main_frame.hpp"
#include "guimanager/i_menu_helper.hpp"
#include "math/vector2.hpp"
#include "math/vector3.hpp"

#include <memory>

class EditorMainFrame : public IBWMainFrame
{
public:
	EditorMainFrame();
	
	BW::Vector2 currentCursorPosition() const override;
	BW::Vector3 getWorldRay(int x, int y) const override;
	bool cursorOverGraphicsWnd() const override;

	void setEditorApp( IEditorApp* editorApp ) override;
	IEditorApp* getEditorApp() override;
	void setEditorView( GLView* glView ) override;
	GLView* getEditorView() override;
	const GLView* getEditorView() const override;
	BW::GUI::IMenuHelper* getMenuHelper() override;
	void setMenuHelper( BW::GUI::IMenuHelper* menuHelper ) override;

	void setMessageText( const wchar_t * pText ) override;
	void setStatusText( unsigned int id, const wchar_t * text ) override;

private:
	IEditorApp * editorApp_;
	GLView * glView_;
	std::unique_ptr<BW::GUI::IMenuHelper> menuHelper_;
};

#endif//EDITOR_MAIN_FRAME_HPP