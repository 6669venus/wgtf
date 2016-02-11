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
	entry_.reset();
}

Connection & Connection::operator=(Connection && other)
{
	entry_ = std::move(other.entry_);
	return *this;
}

void Connection::enable()
{
	if (entry_)
	{
		std::lock_guard<std::mutex> lock(entry_->mutex_);
		entry_->enabled_ = true;
	}
}

void Connection::disable()
{
	if (entry_)
	{
		std::lock_guard<std::mutex> lock(entry_->mutex_);
		entry_->enabled_ = false;
	}
}

void Connection::disconnect()
{
	entry_.reset();
}

bool Connection::enabled() const
{
	return entry_ && entry_->enabled_;
}

bool Connection::connected() const
{
	return static_cast<bool>(entry_);
}
