#ifndef I_DATASTREAM_HPP
#define I_DATASTREAM_HPP
#include "core_variant/type_id.hpp"
#include "core_variant/variant.hpp"
#include <assert.h>

class IDataStream
{
public:
	virtual ~IDataStream() {}

	virtual void seek( size_t pos ) = 0;
	virtual size_t pos() const = 0;
	virtual size_t size() const = 0;
	virtual const void * rawBuffer() const = 0;
	virtual size_t readRaw( void * o_Data, size_t length ) = 0;
	virtual size_t writeRaw( const void * data, size_t length ) = 0;

	virtual bool eof() const
	{
		return ( pos() == size() );
	}

	template< typename T>
	typename std::enable_if<!std::is_pointer<T>::value && variant::traits<T>::can_downcast, bool>::type
		read( T & t )
	{
		bool br = false;
		Variant variant = t;
		br = read( variant );
		if(br)
		{
			t = variant.cast<T>();
		}
		else
		{
			assert( false );
		}
		return br;
	}

	bool read( Variant & variant )
	{
		return readValue( variant );
	}

	template< typename T >
	typename std::enable_if<!std::is_pointer<T>::value && variant::traits<T>::can_upcast, bool>::type
		write( const T & t )
	{
		Variant variant = t;
		return write( variant );
	}

	bool write( const Variant & variant )
	{
		return writeValue( variant );
	}

private:
	virtual bool writeValue( const Variant & variant ) = 0;
	virtual bool readValue( Variant & variant ) = 0;

};

#endif