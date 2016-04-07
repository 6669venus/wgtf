#ifndef SIMPLE_API_FOR_XML_HPP_INCLUDED
#define SIMPLE_API_FOR_XML_HPP_INCLUDED

#include "core_serialization/serialization_dll.hpp"
#include "core_serialization/text_stream.hpp"
#include <memory>

#pragma warning (push)
#pragma warning (disable : 4251) // * needs to have dll-interface to be used by clients of class '*'

class SERIALIZATION_DLL SimpleApiForXml
{
public:
	explicit SimpleApiForXml( TextStream& stream );
	~SimpleApiForXml();

	bool parse();

protected:
	// Not just `abort()` to avoid accidental `::abort()` call.
	void abortParsing();
	bool aborted() const;

private:
	class Impl;
	std::unique_ptr< Impl > impl_;

	virtual void elementStart( const char* name, const char* const* attributes );
	virtual void elementEnd( const char* name );
	virtual void characterData( const char* data, size_t length );

};

#pragma warning (pop)

#endif
