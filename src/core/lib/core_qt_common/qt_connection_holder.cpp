#include "qt_connection_holder.hpp"

#include <assert.h>

QtConnectionHolder::QtConnectionHolder()
{
}

QtConnectionHolder::QtConnectionHolder( QMetaObject::Connection& connection )
{
	*this += connection;
}

QtConnectionHolder::~QtConnectionHolder()
{
	reset();
}

void QtConnectionHolder::reset()
{
	for (auto connection : connections_)
	{
		QObject::disconnect( connection );
	}
}

QtConnectionHolder& QtConnectionHolder::operator+=( const QMetaObject::Connection& connection )
{
	assert( connection );
	connections_.push_back( connection );
	return *this;
}