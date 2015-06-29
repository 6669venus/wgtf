#ifndef TEXT_STREAM_HPP
#define TEXT_STREAM_HPP

#include "i_datastream.hpp"
#include <unordered_map>
#include <sstream>

class ISerializationManager;
class TextStream
	: public IDataStream
{
public:
	TextStream( const std::string & str = "", std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out);

	//From IDataStream
	void seek( size_t pos ) override;
	size_t pos() const override;
	size_t size() const override;
	const void * rawBuffer() const override;
	size_t readRaw( void * o_Data, size_t length ) override;
	size_t writeRaw( const void * data, size_t length ) override;
	bool eof() const override;

	std::string getData()
	{
		return stream_.str();
	}

private:
	bool writeValue( const Variant & variant );
	bool readValue( Variant & variant );

	std::stringstream stream_;
};

#endif //TEXT_STREAM_HPP