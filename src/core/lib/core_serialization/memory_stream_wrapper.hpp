#ifndef MEMORY_STREAM_WRAPPER_HPP
#define MEMORY_STREAM_WRAPPER_HPP

#include "i_datastream.hpp"

class ISerializationManager;

class MemoryStreamWrapper
	: public IDataStream
{
public:
	MemoryStreamWrapper( char * buffer, size_t size );

	//From IDataStream
	void seek( size_t pos ) override;
	size_t pos() const override;
	size_t size() const override;
	const void * rawBuffer() const override;
	size_t readRaw( void * o_Data, size_t length ) override;
	size_t writeRaw( const void * data, size_t length ) override;
private:
	bool writeValue( const Variant & variant ) override;
	bool readValue( Variant & variant ) override;

	char *	buffer_;
	size_t	pos_;
	size_t	size_;
};

#endif //MEMORY_STREAM_WRAPPER_HPP