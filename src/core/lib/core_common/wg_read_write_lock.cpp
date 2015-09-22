#include "wg_read_write_lock.hpp"


wg_read_write_lock::wg_read_write_lock()
	: writer_( false )
	, readers_( 0 )
	, mutex_()
	, unlocked_()
{
}


wg_read_write_lock::~wg_read_write_lock()
{
}


void wg_read_write_lock::read_lock()
{
	std::unique_lock< std::mutex > lock( mutex_ );

	while (writer_)
	{
		// Write in progress, wait until it is done
		unlocked_.wait( lock );
	}
	++readers_;

	lock.unlock();
}


void wg_read_write_lock::read_unlock()
{
	std::unique_lock< std::mutex > lock( mutex_ );

	--readers_;
	if (readers_ == 0)
	{
		// Notify all waiters
		unlocked_.notify_all();
	}

	lock.unlock();
}


void wg_read_write_lock::write_lock()
{
	std::unique_lock< std::mutex > lock( mutex_ );

	while (writer_ || (readers_ > 0))
	{
		// Write or read in progress, wait until it is done
		unlocked_.wait( lock );
	}
	writer_ = true;

	lock.unlock();
}


void wg_read_write_lock::write_unlock()
{
	std::unique_lock< std::mutex > lock( mutex_ );

	writer_ = false;

	// The lock is available now, notify all waiters
	unlocked_.notify_all();

	lock.unlock();
}

