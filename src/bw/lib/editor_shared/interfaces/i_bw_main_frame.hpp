#ifndef I_BW_MAIN_FRAME_HPP
#define I_BW_MAIN_FRAME_HPP

#include "editor_shared/gui/i_main_frame.hpp"

class IEditorApp;
class GLView;

namespace BW
{
	namespace GUI
	{
		class IMenuHelper;
	}

	class Vector2;
	class Vector3;
}

class IBWMainFrame
	: public IMainFrame
{
public:
	virtual BW::Vector2 currentCursorPosition() const = 0;
	virtual BW::Vector3 getWorldRay(int x, int y) const = 0;
	virtual bool cursorOverGraphicsWnd() const = 0;

	virtual void setEditorApp( IEditorApp* editorApp ) {}
	virtual IEditorApp* getEditorApp() { return nullptr; }
	virtual void setEditorView( GLView* glView ) { }
	virtual GLView* getEditorView() { return nullptr; }
	virtual const GLView* getEditorView() const { return nullptr; }
	virtual BW::GUI::IMenuHelper* getMenuHelper() { return nullptr; }
	virtual void setMenuHelper( BW::GUI::IMenuHelper* menuHelper ) {}
};

#endif //I_BW_MAIN_FRAME_HPP