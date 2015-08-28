#include "qt_application_adapter.hpp"

QtApplicationAdapter::QtApplicationAdapter()
{
}

QtApplicationAdapter::~QtApplicationAdapter()
{
}

int QtApplicationAdapter::startApplication()
{
	assert( application_ != nullptr );
	application_->processEvents();
}

void QtApplicationAdapter::registerListener( IApplicationListener * listener )
{
	listeners_.push_back( listener );
}


void QtApplicationAdapter::deregisterListener( IApplicationListener * listener )
{
	auto && listenerIt = std::find( listeners_.begin(), listeners_.end(), listener );
	assert( listenerIt != listeners_.end() );
	listeners_.erase( listenerIt );
}

