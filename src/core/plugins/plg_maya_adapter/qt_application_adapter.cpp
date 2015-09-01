#include "qt_application_adapter.hpp"
#include <QtGui/QGuiApplication>
#include <QApplication>
#include <assert.h>

QtApplicationAdapter::QtApplicationAdapter()
{
	//QObject::connect( QGuiApplication::instance(),
	//	&QGuiApplication::applicationStateChanged,
	//	this,
	//	[&]( Qt::ApplicationState state ) {
	//		if (state == Qt::ApplicationActive)
	//		{
	//			for(auto & listener : this->listeners_)
	//			{
	//				listener->applicationStarted();
	//			}
	//		}
	//	});

	//QObject::connect( QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, [&]() {
	//	for(auto & listener : this->listeners_)
	//	{
	//		listener->applicationStopped();
	//	}
	//});
}

QtApplicationAdapter::~QtApplicationAdapter()
{
	for (auto listener : listeners_)
	{
		listener->applicationStopped();
	}
}

int QtApplicationAdapter::startApplication()
{
	assert( application_ != nullptr );
	application_->processEvents();
	return 0;
}

void QtApplicationAdapter::addListener( IApplicationListener * listener )
{
	listeners_.push_back( listener );
}

void QtApplicationAdapter::removeListener( IApplicationListener * listener )
{
	auto && listenerIt = std::find( listeners_.begin(), listeners_.end(), listener );
	assert( listenerIt != listeners_.end() );
	listeners_.erase( listenerIt );
}

