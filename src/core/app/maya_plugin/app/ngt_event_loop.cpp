#include "ngt_event_loop.hpp"
#include <QtCore/QTimer>
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_logging/logging.hpp"

NGTEventLoop::NGTEventLoop( IApplication* application, QObject* parent)
	: timer_( new QTimer( this ) )
	, application_( application )
{
	QObject::connect( timer_, SIGNAL( QTimer::timeout() ), this, SLOT( NGTEventLoop::processEvents() ) );
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
