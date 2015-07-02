#include "ngt_event_loop.hpp"
#include <QtCore/QTimer>
#include "generic_plugin/interfaces/i_application.hpp"

NGTEventLoop::NGTEventLoop( IApplication* application, QObject* parent)
	: timer_( new QTimer( this ) )
	, application_( application )
{
	connect( timer_, SIGNAL(timeout()), this, SLOT(processEvents()));
}

void NGTEventLoop::processEvents()
{
	application_->processEvents();
}

void NGTEventLoop::start()
{
	timer_->start( 0 );
}

void NGTEventLoop::stop()
{
	timer_->stop();
}
