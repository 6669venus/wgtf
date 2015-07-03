#include "qt_gl_view.hpp"
#include "ui_framework/layout_hint.hpp"

#include "cursor/cursor.hpp"
#include "input/input.hpp"
#include "bw_qt_common/app/i_editor_app.hpp"

#include <QMouseEvent>

namespace
{

	BW::KeyCode::Key getMouseButton( const Qt::MouseButton& mouseButton )
	{
		if (mouseButton == Qt::LeftButton)
		{
			return BW::KeyCode::KEY_LEFTMOUSE;
		}
		else if (mouseButton == Qt::RightButton)
		{
			return BW::KeyCode::KEY_RIGHTMOUSE;
		}
		else if (mouseButton == Qt::MiddleButton)
		{
			return BW::KeyCode::KEY_MIDDLEMOUSE;
		}
		return BW::KeyCode::KEY_NONE;
	}


} // namespace


class QtGLViewImpl
{
	friend QtGLView;

	Cursor::CursorExplicitlyChanged cursorChangedListener_;
};


QtGLView::QtGLView( QWidget* parent )
	: QWidget( parent )
	, resized_( true )
	, lastPos_( FLT_MAX, FLT_MAX )
	, pImpl_( new QtGLViewImpl() )
	, hint_( "3D" )
{
	this->setAttribute( Qt::WA_PaintOnScreen );

	pImpl_->cursorChangedListener_ =
		std::bind(
		&QtGLView::cursorPositionChanged, this,
		std::placeholders::_1,
		std::placeholders::_2 );
	Cursor::addPositionChangedListener( pImpl_->cursorChangedListener_ );
}


QtGLView::~QtGLView()
{
	delete pImpl_;
}


const char * QtGLView::title() const
{
	return "3DView";
}


const char * QtGLView::windowId() const
{
	return nullptr;
}

void* QtGLView::nativeWindowId()
{
	return getNativeId();
}

const LayoutHint& QtGLView::hint() const
{
	return hint_;
}


void QtGLView::update()
{
	if (editorApp_ != nullptr)
	{
		editorApp_->onIdle();
	}
}


QPaintEngine* QtGLView::paintEngine() const
{
	return nullptr;
}


void QtGLView::paintEvent( QPaintEvent* event )
{
	if (!this->paint( resized_ ))
	{
		QWidget::paintEvent( event );
		return;
	}
	resized_ = false;
}


void QtGLView::resizeEvent( QResizeEvent* event )
{
	resized_ = true;
}


void QtGLView::mouseMoveEvent( QMouseEvent* event )
{
	BW::Vector2 mousePos( event->x(), event->y() );

	BW::Vector2 globalPos( event->globalX(), event->globalY() );

	int dx = lastPos_.x == FLT_MAX ? 0 : globalPos.x - lastPos_.x;
	int dy = lastPos_.y == FLT_MAX ? 0 : globalPos.y - lastPos_.y;
	BW::InputDevices::instance().pushMouseEvent(
		dx, dy, 0, mousePos );
	lastPos_ = globalPos;
}


void QtGLView::mousePressEvent( QMouseEvent* event )
{
	const BW::Vector2 mousePos( event->x(), event->y() );
	const Qt::MouseButton mouseButton = event->button();
	const BW::KeyCode::Key button = getMouseButton( mouseButton );
	BW::InputDevices::instance().pushMouseButtonEvent( 
		button, true, mousePos );
}


void QtGLView::mouseReleaseEvent( QMouseEvent* event )
{
	const BW::Vector2 mousePos( event->x(), event->y() );
	const Qt::MouseButton mouseButton = event->button();
	const BW::KeyCode::Key button = getMouseButton( mouseButton );
	BW::InputDevices::instance().pushMouseButtonEvent( 
		button, false, mousePos );
}


void QtGLView::keyPressEvent( QKeyEvent* event )
{
	QPoint point = QCursor::pos();
	BW::Vector2 mousePos( point.x(), point.y() );
	WPARAM nativeScanCode = event->nativeVirtualKey();
	BW::KeyCode::Key key =
		BW::InputDevices::instance().getKeyCodeDown(
		nativeScanCode, false );
	BW::InputDevices::instance().pushKeyEvent( key, true, mousePos );
}


void QtGLView::keyReleaseEvent( QKeyEvent* event )
{
	QPoint point = QCursor::pos();
	BW::Vector2 mousePos( point.x(), point.y() );
	WPARAM nativeScanCode = event->nativeVirtualKey();
	BW::KeyCode::Key key =
		BW::InputDevices::instance().getKeyCodeUp(
		nativeScanCode, true );
	BW::InputDevices::instance().pushKeyEvent( key,  false, mousePos );
}


void QtGLView::wheelEvent( QWheelEvent* wheelEvent )
{
	BW::Vector2 mousePos( wheelEvent->x(), wheelEvent->y() );
	BW::InputDevices::instance().pushMouseEvent(
		0, 0, wheelEvent->delta(), mousePos );
}


void QtGLView::cursorPositionChanged( int x, int y )
{
	lastPos_.x = x;
	lastPos_.y = y;
}


//==============================================================================
bool QtGLView::underMouse() const
{
	return QWidget::underMouse();
}


//==============================================================================
void* QtGLView::getNativeId() const
{
	return reinterpret_cast<void*>( QWidget::winId() );
}
