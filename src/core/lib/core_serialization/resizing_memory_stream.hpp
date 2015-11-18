#ifndef RESIZING_MEMORY_STREAM_HPP
#define RESIZING_MEMORY_STREAM_HPP

#include "i_datastream.hpp"
#include <unordered_map>
#include <vector>

typedef std::unordered_map< const TypeId, std::function< void (IDataStream&, const Variant &)> > WriteFuncMap;
typedef std::unordered_map< const TypeId, std::function< void (IDataStream&, Variant &)> > ReadFuncMap;

class ResizingMemoryStream
	: public IDataStream
{
public:

	ResizingMemoryStream( size_t capacity = 0 );
	ResizingMemoryStream( const char * data, size_t size );
	ResizingMemoryStream( ResizingMemoryStream && other );
	~ResizingMemoryStream();

	void resetData();
	//From IDataStream
	void seek( size_t pos ) override;
	size_t pos() const override;
	size_t size() const override;
	const void * rawBuffer() const override;
	size_t writeRaw( const void * value, size_t length = 0 ) override;
	size_t readRaw( void * o_Value, size_t length = 0 ) override;

private:
	ResizingMemoryStream( ResizingMemoryStream & other );
	ResizingMemoryStream & operator= ( ResizingMemoryStream && rhs );
	ResizingMemoryStream & operator= ( ResizingMemoryStream & rhs );

	void init();

	bool writeValue( const Variant & variant ) override;
	bool readValue( Variant & variant ) override;

	size_t				pos_;
	std::vector< char >	buffer_;
	WriteFuncMap writeFuncMap_;
	ReadFuncMap readFuncMap_;

};

#endif //RESIZING_MEMORY_STREAM_HPP

