#ifndef QT_GL_VIEW_H
#define QT_GL_VIEW_H

#include <QWidget>
#include "editor_shared/view/gl_view.hpp"
#include "math/vector2.hpp"
#include "ui_framework/i_view.hpp"

class QtGLView
	: public QWidget
	, public GLView
	, public IView
{
public:
	QtGLView( QWidget* parent );
	~QtGLView();

	const char * title() const override;
	const char * windowId() const override;
	LayoutHint hint() const override;

	void update() override;

	QPaintEngine* paintEngine () const;

	void paintEvent( QPaintEvent* event );
	void resizeEvent( QResizeEvent* event );

	void mouseMoveEvent( QMouseEvent* event );
	void mousePressEvent( QMouseEvent* event );
	void mouseReleaseEvent( QMouseEvent* event );

	void wheelEvent( QWheelEvent* );

	void keyPressEvent( QKeyEvent* );
	void keyReleaseEvent( QKeyEvent* );

	bool underMouse() const;

	void* getNativeId () const;

private:
	void cursorPositionChanged( int x, int y );

	bool resized_;
	BW::Vector2 lastPos_;

	class QtGLViewImpl* pImpl_;
};

#endif