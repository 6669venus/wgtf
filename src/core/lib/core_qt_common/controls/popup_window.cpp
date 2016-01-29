#include "popup_window.hpp"
#include "../helpers/qt_helpers.hpp"

#include <QApplication>
#include <QCursor>
#include <QQuickItem>

namespace
{
	class PopupEventFilter
		: public QObject
	{
	public:
		bool eventFilter( QObject * object, QEvent * event )
		{
			QEvent::Type type = event->type();
			if (type == QEvent::WindowDeactivate||
				type == QEvent::ApplicationDeactivate) 
			{
				popup_->close();
				return true;
			}
			else if( event->type() == QEvent::MouseButtonPress &&
				object != popup_ &&
				popup_->findChildren< QObject * >().contains( object ) == false )
			{
				popup_->close();
				return true;
			}
			// standard event processing
			return QObject::eventFilter(object, event );
		};

		void setPopup( QWindow * window )
		{
			popup_ = window;
		}

	private:
		QWindow * popup_;
	};

	PopupEventFilter s_PopupEventFilter;
}

//==============================================================================
PopupWindow::PopupWindow()
	: anchor_( NULL )
{
	connect(
		this, &QWindow::visibleChanged,
		this, &PopupWindow::onVisibleChanged );
}


//==============================================================================
PopupWindow::~PopupWindow()
{
	disconnect(
		this, &QWindow::visibleChanged,
		this, &PopupWindow::onVisibleChanged );
}


//==============================================================================
QVariant PopupWindow::getAnchor()
{
	return QVariant::fromValue< QQuickItem * >( anchor_ );
}


//==============================================================================
void PopupWindow::setAnchor( QVariant anchor )
{
	anchor_ = qvariant_cast< QQuickItem * >( anchor );

	int x = 0;
	int y = 0;

	QObject * qObject = anchor_;
	while( qObject != NULL )
	{
		QQuickItem * item = qobject_cast< QQuickItem * >( qObject );
		if( item != NULL )
		{
			x += item->x();
			y += item->y();
		}
		else
		{
			QWindow * window = qobject_cast< QWindow * >( qObject );
			if (window != NULL)
			{
				x += window->x();
				y += window->y();
			}
		}
		QObject * parent = item ? item->parentItem() : qObject->parent();
		if ( parent == NULL && item && item->window() )
		{
			parent = item->window();
		}
		qObject = parent;
	}

	setPosition( x, y );
}


//==============================================================================
/*virtual */void PopupWindow::mousePressEvent(QMouseEvent * event)
{
	QQuickItem * dropDownList =
		QtHelpers::findChildByObjectName( this, "dropDownList" );
	if (dropDownList != NULL)
	{
		QQuickItem * child = dropDownList->childAt( event->x(), event->y() );
		if (child != NULL)
		{
			QMetaObject::invokeMethod( this, "menuSelect" ); 
		}
		return;
	}
	QQuickWindow::mousePressEvent( event );
}


//==============================================================================
void PopupWindow::onVisibleChanged( bool visible )
{
	if (visible)
	{
		QApplication::instance()->installEventFilter( &s_PopupEventFilter );
		s_PopupEventFilter.setPopup( this );
	}
	else
	{
		QApplication::instance()->removeEventFilter( &s_PopupEventFilter );
		s_PopupEventFilter.setPopup( NULL );
	}
}