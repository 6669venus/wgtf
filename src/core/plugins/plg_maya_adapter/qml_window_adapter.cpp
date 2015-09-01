#include "qml_window_adapter.hpp"
#include <QtWidgets/QWidget>
#include <assert.h>

QmlWindowAdapter::QmlWindowAdapter( IQtFramework & qtFramework, QQmlEngine & qmlEngine )
	: QmlWindow( qtFramework, qmlEngine )
{
}

QmlWindowAdapter::~QmlWindowAdapter()
{
}

void QmlWindowAdapter::close()
{
	for (auto listener : listeners_)
	{
		listener->windowClosed( this );
	}

	QmlWindow::close();
}

void QmlWindowAdapter::show()
{
	for (auto listener : listeners_)
	{
		listener->windowShow( this );
	}

	QmlWindow::show();
}

void QmlWindowAdapter::hide()
{
	for (auto listener : listeners_)
	{
		listener->windowHide( this );
	}
	QmlWindow::hide();
}


void * QmlWindowAdapter::nativeWindowId() const
{
	return reinterpret_cast< void * >( mainWindow()->winId() );
}

void QmlWindowAdapter::makeFramelessWindow()
{
	mainWindow()->setWindowFlags( Qt::Widget | Qt::FramelessWindowHint );
}


void QmlWindowAdapter::addListener( IWindowListener * listener )
{
	listeners_.push_back( listener );
}

void QmlWindowAdapter::removeListener( IWindowListener * listener )
{
	auto && listenerIt = std::find( listeners_.begin(), listeners_.end(), listener );
	assert( listenerIt != listeners_.end() );
	listeners_.erase( listenerIt );
}
