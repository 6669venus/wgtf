#include "resizing_memory_stream.hpp"
#include <cstring>
#include <algorithm>
#include <utility>


ResizingMemoryStream::ResizingMemoryStream():
	buffer_(),
	pos_( 0 )
{
}


ResizingMemoryStream::ResizingMemoryStream( Buffer buffer ):
	buffer_( std::move( buffer ) ),
	pos_( 0 )
{
}


ResizingMemoryStream::ResizingMemoryStream( ResizingMemoryStream&& that ):
	buffer_( std::move( that.buffer_ ) ),
	pos_( that.pos_ )
{
	that.pos_ = 0;
}


ResizingMemoryStream& ResizingMemoryStream::operator=( ResizingMemoryStream&& that )
{
	buffer_ = std::move( that.buffer_ );
	pos_ = that.pos_;

	that.pos_ = 0;

	return *this;
}


const ResizingMemoryStream::Buffer& ResizingMemoryStream::buffer() const
{
	return buffer_;
}


ResizingMemoryStream::Buffer ResizingMemoryStream::takeBuffer()
{
	pos_ = 0;
	return std::move( buffer_ );
}


void ResizingMemoryStream::setBuffer( Buffer buffer )
{
	buffer_ = std::move( buffer );
	pos_ = 0;
}


void ResizingMemoryStream::clear()
{
	buffer_.clear();
	pos_ = 0;
}


std::streamoff ResizingMemoryStream::seek( std::streamoff offset, std::ios_base::seekdir dir )
{
	std::streamoff pos;
	switch (dir)
	{
	case std::ios_base::beg:
		pos = offset;
		break;

	case std::ios_base::cur:
		pos = pos_ + offset;
		break;

	case std::ios_base::end:
		pos = static_cast<std::streamoff>( buffer_.size() ) + offset;
		break;

	default:
		return -1;

	}

	if (pos < 0 ||
		pos > static_cast<std::streamoff>( buffer_.size() ))
	{
		return -1;
	}

	pos_ = pos;

	return pos_;
}


std::streamsize ResizingMemoryStream::read( void* destination, std::streamsize size )
{
	const std::streamsize toRead = std::min(
		size,
		static_cast<std::streamoff>( buffer_.size() ) - pos_ );
	if (toRead > 0)
	{
		std::memcpy( destination, &buffer_[pos_], toRead );
		pos_ += toRead;
	}

	return toRead;
}


std::streamsize ResizingMemoryStream::write( const void* source, std::streamsize size )
{
	std::streamsize newPos = pos_ + size;
	if (newPos > static_cast<std::streamoff>( buffer_.size() ))
	{
		buffer_.resize( newPos );
	}

	std::memcpy( &buffer_[pos_], source, size );
	pos_ = newPos;

	return size;
}


bool ResizingMemoryStream::sync()
{
	return true;
}


