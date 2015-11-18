#include "resizing_memory_stream.hpp"
#include "wg_types/binary_block.hpp"
#include "core_logging/logging.hpp"
#include <assert.h>
#include <sstream>

namespace
{
	
	//------------------------------------------------------------------------------
	void writeInt64( IDataStream& stream, const Variant & variant )
	{
		size_t writeData = 0;
		size_t length = sizeof(int64_t);
		int64_t value = variant.castRef<int64_t>();
		writeData = stream.writeRaw( &value, length );
		assert( writeData == length );
	}


	//------------------------------------------------------------------------------
	void writeUInt64( IDataStream& stream, const Variant & variant )
	{
		size_t writeData = 0;
		size_t length = sizeof(uint64_t);
		uint64_t value = variant.castRef<uint64_t>();
		writeData = stream.writeRaw( &value, length );
		assert( writeData == length );
	}


	//------------------------------------------------------------------------------
	void writeDouble( IDataStream& stream, const Variant & variant )
	{
		size_t writeData = 0;
		size_t length = sizeof(double);
		double value = variant.castRef<double>();
		writeData = stream.writeRaw( &value, length );
		assert( writeData == length );
	}


	//------------------------------------------------------------------------------
	void writeString( IDataStream& stream, const Variant & variant )
	{
		size_t writeData = 0;
		const auto & value = variant.castRef<std::string>();
		size_t length = value.length();
		stream.write( length );
		writeData = stream.writeRaw( value.c_str(), length );
		assert( writeData == length );
	}

	//------------------------------------------------------------------------------
	void writeBinaryPtr( IDataStream& stream, const Variant & variant )
	{
		const auto & binary = variant.castRef<std::shared_ptr< BinaryBlock > >();
		stream.write( binary->length() );
		stream.writeRaw( binary->data(), binary->length() );
	}


	//------------------------------------------------------------------------------
	void readInt64( IDataStream& stream, Variant & variant )
	{
		uint64_t readData = 0;
		size_t length = sizeof(int64_t);
		int64_t value;
		readData = stream.readRaw( &value, length );
		variant = value;
		assert( readData == length );
	}


	//------------------------------------------------------------------------------
	void readUInt64( IDataStream& stream, Variant & variant )
	{
		uint64_t readData = 0;
		size_t length = sizeof(uint64_t);
		uint64_t value;
		readData = stream.readRaw( &value, length );
		variant = value;
		assert( readData == length );
	}


	//------------------------------------------------------------------------------
	void readDouble( IDataStream& stream, Variant & variant )
	{
		uint64_t readData = 0;
		size_t length = sizeof(double);
		double value;
		readData = stream.readRaw( &value, length );
		variant = value;
		assert( readData == length );
	}


	//------------------------------------------------------------------------------
	void readString( IDataStream& stream, Variant & variant )
	{
		uint64_t readData = 0;
		size_t length = UINTPTR_MAX;
		std::string value( "" );
		stream.read( length );
		value.resize( length );
		readData = stream.readRaw( (void *)( value.c_str() ), length );
		variant = value;
		assert( readData == length );
	}

	//------------------------------------------------------------------------------
	void readBinaryPtr( IDataStream& stream, Variant & variant )
	{
		int length = -1;
		stream.read( length );
		auto binary = std::make_shared< BinaryBlock >( nullptr, length, false );
		stream.readRaw( const_cast<void *>( binary->data() ), length );
		variant = binary;
	}


	//------------------------------------------------------------------------------
	void getWriteFuncMap( WriteFuncMap& writeFuncMap )
	{
		if (!writeFuncMap.empty())
		{
			return;
		}

		static TypeId int64Type = TypeId::getType<int64_t>();
		static TypeId uint64Type = TypeId::getType<uint64_t>();
		static TypeId doubleType = TypeId::getType<double>();
		static TypeId stringType = TypeId::getType<std::string>();
		static TypeId binaryPtrType = TypeId::getType<std::shared_ptr< BinaryBlock>>();
		writeFuncMap[int64Type] = 
			std::bind( writeInt64, std::placeholders::_1, std::placeholders::_2 );
		writeFuncMap[uint64Type] = 
			std::bind( writeUInt64, std::placeholders::_1, std::placeholders::_2 );
		writeFuncMap[doubleType] = 
			std::bind( writeDouble, std::placeholders::_1, std::placeholders::_2 );
		writeFuncMap[stringType] = 
			std::bind( writeString, std::placeholders::_1, std::placeholders::_2 );
		writeFuncMap[binaryPtrType] = 
			std::bind( writeBinaryPtr, std::placeholders::_1, std::placeholders::_2 );
	}


	//------------------------------------------------------------------------------
	void getReadFuncMap( ReadFuncMap& readFuncMap )
	{
		if (!readFuncMap.empty())
		{
			return;
		}

		static TypeId int64Type = TypeId::getType<int64_t>();
		static TypeId uint64Type = TypeId::getType<uint64_t>();
		static TypeId doubleType = TypeId::getType<double>();
		static TypeId stringType = TypeId::getType<std::string>();
		static TypeId binaryPtrType = TypeId::getType<std::shared_ptr< BinaryBlock>>();
		readFuncMap[int64Type] = 
			std::bind( readInt64, std::placeholders::_1, std::placeholders::_2 );
		readFuncMap[uint64Type] = 
			std::bind( readUInt64, std::placeholders::_1, std::placeholders::_2 );
		readFuncMap[doubleType] = 
			std::bind( readDouble, std::placeholders::_1, std::placeholders::_2 );
		readFuncMap[stringType] = 
			std::bind( readString, std::placeholders::_1, std::placeholders::_2 );
		readFuncMap[binaryPtrType] = 
			std::bind( readBinaryPtr, std::placeholders::_1, std::placeholders::_2 );
	}

}


//==============================================================================
ResizingMemoryStream::ResizingMemoryStream( size_t capacity /*= 0 */)
	: pos_( 0 )
{
	init();
	buffer_.reserve( capacity );
}

ResizingMemoryStream::ResizingMemoryStream( const char * data, size_t size )
	: pos_( 0 )
{
	init();
	buffer_.resize( size );
	buffer_.assign( data, data + size );
}

ResizingMemoryStream::ResizingMemoryStream( ResizingMemoryStream && other )
	: pos_( other.pos_ )
	, buffer_( std::move( other.buffer_ ))
	, writeFuncMap_( std::move( other.writeFuncMap_ ) )
	, readFuncMap_( std::move( other.readFuncMap_ ) )
{
}

ResizingMemoryStream::~ResizingMemoryStream()
{
}

void ResizingMemoryStream::init()
{
	getWriteFuncMap( writeFuncMap_ );
	getReadFuncMap( readFuncMap_ );
}

void ResizingMemoryStream::resetData()
{
	buffer_.clear();
	seek( 0 );
}

//==============================================================================
/*virtual */void ResizingMemoryStream::seek( size_t pos )
{
	pos_ = pos;
	if( pos_ > size() )
	{
		pos_ = size();
	}
}


//==============================================================================
/*virtual */size_t ResizingMemoryStream::pos() const
{
	return pos_;
}


//==============================================================================
/*virtual */size_t ResizingMemoryStream::size() const
{
	return buffer_.size();
}

const void * ResizingMemoryStream::rawBuffer() const
{
	return reinterpret_cast< const void* >( buffer_.data() );
}

//==============================================================================
bool ResizingMemoryStream::readValue( Variant & variant )
{
	assert( !readFuncMap_.empty() );
	const TypeId& type = variant.type()->typeId();
	auto findIt = readFuncMap_.find( type );
	if(findIt != readFuncMap_.end())
	{
		findIt->second( *this, variant );
	}
	else
	{
		std::string str;
		this->read( str );
		std::stringstream stream( str );
		Variant tmp(variant.type());
		stream >> tmp;
		if (!stream.fail())
		{
			if (variant.type() != tmp.type())
			{
				NGT_WARNING_MSG("Resizing mem stream has type '%s', expected '%s'\n", tmp.type()->name(), variant.type()->name());
			}
			variant = std::move( tmp );
		}
		else
		{
			NGT_ERROR_MSG("Malformed data in resizing mem stream '%s'\n", str.c_str());
			return false;
		}
	}
	return true;
}


//==============================================================================
bool ResizingMemoryStream::writeValue( const Variant & variant )
{
	assert( !writeFuncMap_.empty() );
	TypeId type( variant.type()->name() );
	auto findIt = writeFuncMap_.find( type );
	if (findIt != writeFuncMap_.end())
	{
		findIt->second( *this, variant );
	}
	else
	{
		std::stringstream stream;
		stream << variant;
		if (!stream.good())
		{
			assert( false );
			return false;
		}
		return this->write( stream.str() );
	}
	return true;
}


//==============================================================================
size_t ResizingMemoryStream::readRaw( void * o_Value, size_t length )
{
	if(!length)
	{
		return 0;
	}
	if( pos_ + length > size() )
	{
		return 0;
	}
	memcpy( o_Value, &buffer_[ 0 ] + pos_, length );
	pos_ += length;
	return length;
}

//==============================================================================
size_t ResizingMemoryStream::writeRaw( const void * value, size_t length )
{
	if(!length)
	{
		return 0;
	}
	size_t currentSize = size();
	if( pos_ + length > currentSize )
	{
		buffer_.resize( currentSize + ( length - ( currentSize - pos_ ) ) );
	}
	memcpy( &buffer_[ 0 ] + pos_, value, length );
	pos_ += length;
	return length;
}
