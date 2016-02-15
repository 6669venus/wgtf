#include "signal.hpp"

Connection::Connection()
{
}

Connection::Connection(SignalHolderPtr entry) :
	entry_(entry)
{
}

Connection::Connection(Connection && other) : 
	entry_(std::move(other.entry_))
{
}

Connection::~Connection()
{
}

Connection & Connection::operator=(Connection && other)
{
	entry_ = std::move(other.entry_);
	return *this;
}

void Connection::enable()
{
	entry_->enabled_ = true;
}

void Connection::disable()
{
	entry_->enabled_ = false;
}

void Connection::disconnect()
{
	entry_->expired_ = true;
}

bool Connection::enabled() const
{
	return entry_->enabled_;
}

bool Connection::connected() const
{
	return !entry_->expired_;
}

ConnectionHolder::ConnectionHolder()
{

}

ConnectionHolder::~ConnectionHolder()
{
	clear();
}

void ConnectionHolder::clear()
{
	for (auto & connection : connections_)
	{
		connection.disconnect();
	}
	connections_.clear();
}

void ConnectionHolder::add( Connection && connection )
{
	connections_.emplace_back( std::move( connection ) );
}

void ConnectionHolder::operator+=( Connection && connection )
{
	add( std::move( connection ) );
}