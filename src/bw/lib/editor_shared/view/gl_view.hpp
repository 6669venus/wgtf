#ifndef GL_VIEW_H
#define GL_VIEW_H

class IEditorApp;

class GLView
{
public:
	GLView();
	void setEditorApp( IEditorApp* editorApp );
	virtual bool underMouse() const { return false; }
	virtual void * getNativeId() const { return 0; }

protected:
	bool paint( bool resize );
	virtual void resizeWindow();

	IEditorApp* editorApp_;
};

#endif
