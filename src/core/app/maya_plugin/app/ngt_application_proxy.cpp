#include "ngt_application_proxy.hpp"
#include <QtCore/QTimer>
#include "ui_framework/i_ui_application.hpp"
#include "logging/logging.hpp"
#include "maya_window.hpp"

#include <QtGui/QDockWidget>
#include <QtGui/QLayout>
#include <QtGui/QMainWindow>
#include "qwinhost.h"
#include "ui_framework/i_window.hpp"

#include <maya/MQtUtil.h>

NGTApplicationProxy::NGTApplicationProxy( IUIApplication* application, QObject* parent)
	: timer_( new QTimer( this ) )
	, application_( application )
	, mayaWindow_( new MayaWindow() )
	, started_( false )
	, visible_( false )
{
	QObject::connect( timer_, SIGNAL( QTimer::timeout() ), this, SLOT( NGTEventLoop::processEvents() ) );
	application_->registerListener( this );
	application_->addWindow( *mayaWindow_ );
}

void NGTApplicationProxy::applicationStarted()
{

}

void NGTApplicationProxy::applicationStopped()
{
	stop();
}

bool NGTApplicationProxy::started() const
{
	return started_;
}

bool NGTApplicationProxy::visible() const
{
	return visible_;
}

void NGTApplicationProxy::processEvents()
{
	application_->processEvents();
}

void NGTApplicationProxy::start()
{
	auto mw = qobject_cast< QMainWindow * >( MQtUtil::mainWindow() );

	for (auto & kv : application_->windows())
	{
		auto win = kv.second;

		if (win == mayaWindow_)
		{
			continue;
		}

		win->hide();
		win->makeFramelessWindow();

		auto qWidget = new QWinHost( mw );
		HWND winId = reinterpret_cast< HWND >( win->nativeWindowId() );
		qWidget->setWindow( winId );
		qWidget->setWindowTitle( win->title() );
		qWidget->setFeatures( QDockWidget::AllDockWidgetFeatures );
		qWidget->setAllowedAreas( Qt::AllDockWidgetAreas );
		mw->addDockWidget(Qt::RightDockWidgetArea, qWidget );
		win->show();
		windows_.push_back( qWidget );
	}
	started_ = true;
	visible_ = true;

	timer_->start( 0 );
}

void NGTApplicationProxy::stop()
{
	started_ = false;
	visible_ = false;

	timer_->stop();

	for (auto win : windows_)
	{
		win->hide();
		win->deleteLater();
	}

	windows_.clear();
}

void NGTApplicationProxy::show()
{
	for (auto win : windows_)
	{
		win->show();
	}
	visible_ = true;
}

void NGTApplicationProxy::hide()
{
	for (auto win : windows_)
	{
		win->hide();
	}
	visible_ = false;
}

