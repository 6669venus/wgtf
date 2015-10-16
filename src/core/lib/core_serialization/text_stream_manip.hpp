#ifndef TEXT_STREAM_MANIP_HPP_INCLUDED
#define TEXT_STREAM_MANIP_HPP_INCLUDED

#include <string>

class TextStream;


/**
Utility class used to check input stream for exact character(s).
*/
class TextPatternChecker
{
public:
	explicit TextPatternChecker( const char* pattern ):
		pattern_( pattern )
	{
	}

	explicit TextPatternChecker( char pattern ):
		pattern_( buf_ )
	{
		buf_[0] = pattern;
		buf_[1] = 0;
	}

	const char* pattern() const
	{
		return pattern_;
	}

private:
	const char* pattern_;
	char buf_[2];

};

TextStream& operator>>( TextStream& stream, const TextPatternChecker& value );


namespace text_stream_manip_details
{

	/**
	Utility class used for quoted string serialization.
	*/
	class QuotedCStr
	{
	public:
		QuotedCStr( const char* str, size_t size ):
			c_str_( str ),
			size_( size )
		{
		}

		const char* c_str() const
		{
			return c_str_;
		}

		size_t size() const
		{
			return size_;
		}

	private:
		const char* c_str_;
		size_t size_;

	};

	TextStream& operator<<( TextStream& stream, const QuotedCStr& value );


	/**
	Utility class used for quoted string (de)serialization.
	*/
	class QuotedStr
	{
	public:
		explicit QuotedStr( std::string& str ):
			str_( &str )
		{
		}
	
		std::string& str()
		{
			return *str_;
		}

		const std::string& str() const
		{
			return *str_;
		}

	private:
		std::string* str_;

	};

	TextStream& operator<<( TextStream& stream, const QuotedStr& value );
	TextStream& operator>>( TextStream& stream, QuotedStr value );

}

/**
Replacement for C++14 std::quoted() function.

Used to (de)serialize string with quotes, which is useful if string may have
arbitrary contents including spaces, newlines, and special chars.
*/
inline text_stream_manip_details::QuotedCStr quoted( const char* str, size_t size )
{
	return text_stream_manip_details::QuotedCStr( str, size );
}

/**
@overload
*/
inline text_stream_manip_details::QuotedCStr quoted( const char* str )
{
	return text_stream_manip_details::QuotedCStr( str, str ? std::strlen( str ) : 0 );
}

/**
@overload
*/
inline text_stream_manip_details::QuotedCStr quoted( const std::string& str )
{
	return text_stream_manip_details::QuotedCStr( str.c_str(), str.size() );
}

/**
@overload
*/
inline text_stream_manip_details::QuotedStr quoted( std::string& str )
{
	return text_stream_manip_details::QuotedStr( str );
}

#endif

