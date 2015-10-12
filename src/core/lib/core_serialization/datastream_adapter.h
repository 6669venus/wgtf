#ifndef DATASTREAM_ADAPTER_HPP
#define DATASTREAM_ADAPTER_HPP

#include "i_datastream.hpp"

#include <sstream>

/**
 * IDataStream to std:istream adapter
 */
class DataStreamReader : public std::istringstream
{
public:
	DataStreamReader( IDataStream & ds) :
		std::istringstream( read( ds ).c_str() ) 
	{
	}

	static std::string read( IDataStream & ds)
	{
		std::string s;
		ds.read( s );
		return s;
	}
};


/**
 * IDataStream to std::ostream adapter
 */
class DataStreamWriter : public std::ostringstream
{
public:
	DataStreamWriter( IDataStream & ds ) :
		ds_( ds )
	{

	}
	virtual ~DataStreamWriter()
	{
		ds_.write( this->str() );
	}
private:
	IDataStream & ds_;
};

#endif // DATASTREAM_ADAPTER_HPP

