#include "memory_stream_wrapper.hpp"

#include <memory>

//==============================================================================
MemoryStreamWrapper::MemoryStreamWrapper( char * buffer, size_t size )
	: buffer_( buffer )
	, pos_ ( 0 )
	, size_( size )
{
}


//==============================================================================
void MemoryStreamWrapper::seek( size_t pos )
{
	pos_ = pos;
	if( pos_ > size_ )
	{
		pos_ = size_;
	}
}

//==============================================================================
size_t MemoryStreamWrapper::pos() const
{
	return pos_;
}


//==============================================================================
size_t MemoryStreamWrapper::size() const
{
	return size_;
}

const void * MemoryStreamWrapper::rawBuffer() const
{
	return nullptr;
}

bool MemoryStreamWrapper::writeValue( const Variant & variant )
{
	return false;
}
bool MemoryStreamWrapper::readValue( Variant & variant )
{
	return false;
}

//==============================================================================
size_t MemoryStreamWrapper::readRaw( void * o_Data, size_t length )
{
	if( pos_ + length > size_ )
	{
		length = size_ - pos_;
	}
	memcpy( o_Data, buffer_ + pos_, length );
	pos_ += length;
	return length;
}


//==============================================================================
size_t MemoryStreamWrapper::writeRaw( const void * data, size_t length )
{
	if( pos_ + length > size_ )
	{
		length = size_ - pos_;
	}
	memcpy( buffer_ + pos_, data, length );
	pos_ += length;
	return length;
}
