#include "text_stream_manip.hpp"
#include "text_stream.hpp"
#include "fixed_memory_stream.hpp"
#include "resizing_memory_stream.hpp"


TextStream& operator>>( TextStream& stream, const TextPatternChecker& value )
{
	if (!stream.beginReadField())
	{
		return stream;
	}

	const char* pattern = value.pattern();

	while (*pattern && stream.good())
	{
		if (stream.get() != *pattern)
		{
			// pattern mismatch
			stream.setState( std::ios_base::failbit );
			break;
		}

		pattern += 1;
	}

	return stream;
}


namespace text_stream_manip_details
{

	TextStream& operator<<( TextStream& stream, const QuotedCStr& value )
	{
		FixedMemoryStream src( value.c_str(), value.size() );
		stream.serializeString( src );
		return stream;
	}


	TextStream& operator<<( TextStream& stream, const QuotedStr& value )
	{
		FixedMemoryStream src( value.str().c_str(), value.str().size() );
		stream.serializeString( src );
		return stream;
	}


	TextStream& operator>>( TextStream& stream, QuotedStr value )
	{
		ResizingMemoryStream dst;
		stream.deserializeString( dst );
		if (!stream.fail())
		{
			value.str() = dst.takeBuffer();
		}
		return stream;
	}

}


