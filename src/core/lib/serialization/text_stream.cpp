#include "text_stream.hpp"
#include "wg_types/binary_block.hpp"
#include <memory>
#include <streambuf>


//==============================================================================
TextStream::TextStream( const std::string & str, std::ios::openmode mode)
	: stream_( str, mode )
{
	if (!str.empty())
	{
		stream_.seekg( 0 );
	}
}


//==============================================================================
void TextStream::seek( size_t pos )
{
	stream_.seekg( pos );
}

//==============================================================================
size_t TextStream::pos() const
{
	return stream_.cur;
}


//==============================================================================
size_t TextStream::size() const
{
	return  stream_.str().length();
}

const void * TextStream::rawBuffer() const
{
	return stream_.rdbuf();
}

bool TextStream::writeValue( const Variant & variant )
{
	stream_ << variant;
	return stream_.good();
}
bool TextStream::readValue( Variant & variant )
{
	stream_ >> variant;
	return !stream_.fail();
}

//==============================================================================
size_t TextStream::readRaw( void * o_Data, size_t length )
{
	size_t cur = pos();
	stream_.read( (char *)o_Data, length );
	return pos() - cur;
}


//==============================================================================
size_t TextStream::writeRaw( const void * data, size_t length )
{
	size_t cur = pos();
	stream_.write( (const char *)data, length );
	return pos() - cur;
}
