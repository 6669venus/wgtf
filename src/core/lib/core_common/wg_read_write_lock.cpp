#include "wg_read_write_lock.hpp"


wg_read_write_lock::wg_read_write_lock()
	: readers_( 0 )
	, writers_( 0 )
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

	while (0 < writers_)
	{
		// Write in progress, wait until it is done
		unlocked_.wait( lock );
	}
	++readers_;
}


void wg_read_write_lock::read_unlock()
{
	std::unique_lock< std::mutex > lock( mutex_ );

	--readers_;
	if (0 >= readers_)
	{
		readers_ = 0;

		// Notify all waiters
		unlocked_.notify_all();
	}
}


void wg_read_write_lock::write_lock()
{
	std::unique_lock< std::mutex > lock( mutex_ );

	while ((0 < writers_) || (0 < readers_))
	{
		// Write or read in progress, wait until it is done
		unlocked_.wait( lock );
	}

	writeThreadIds_.push_back( std::this_thread::get_id() );
	++writers_;
}


void wg_read_write_lock::write_unlock()
{
	std::unique_lock< std::mutex > lock( mutex_ );

	// See if we have the current threadId in our ids vector
	std::thread::id threadId = std::this_thread::get_id();
	std::vector<std::thread::id>::iterator iter = std::find( writeThreadIds_.begin(), writeThreadIds_.end(), threadId );

	if (writeThreadIds_.end() != iter)
	{
		// Remove the threadId off our vector
		writeThreadIds_.erase( iter, writeThreadIds_.end() );

		--writers_;

		if (0 >= writers_)
		{
			writers_ = 0;

			// The lock is available now, notify all waiters
			unlocked_.notify_all();
		}
	}
}

