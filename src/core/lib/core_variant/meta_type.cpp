#include "meta_type.hpp"
#include "core_serialization/fixed_memory_stream.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/text_stream.hpp"
#include <string>


namespace
{

	bool convertFromString( const MetaType* toType, void* to, const MetaType* fromType, const void* from )
	{
		const std::string& fromStr = *reinterpret_cast<const std::string*>( from );
		FixedMemoryStream dataStream( fromStr.c_str(), fromStr.size() );
		TextStream stream( dataStream );
		toType->streamIn( stream, to );
		return
			!stream.fail() && // conversion succeeded
			stream.peek() == EOF; // whole string was consumed
	}

}


MetaType::MetaType(
	const char* name,
	size_t size,
	const std::type_info& typeInfo,
	const std::type_info* pointedType,
	int flags ):

	typeId_( typeInfo.name() ),
	name_( name ? name : typeInfo.name() ),
	size_( size ),
	typeInfo_( typeInfo ),
	pointedType_( pointedType ),
	flags_( flags ),
	conversionsFrom_(),
	defaultConversionFrom_( nullptr )
{
	addConversionFrom< std::string >( &convertFromString );
}


bool MetaType::convertFrom( void* to, const MetaType* fromType, const void* from ) const
{
	// identity conversion
	if( fromType == this )
	{
		copy( to, from );
		return true;
	}

	// custom conversion
	auto conv = conversionsFrom_.find( &fromType->typeInfo() );
	if( conv != conversionsFrom_.end() )
	{
		return conv->second( this, to, fromType, from );
	}

	// default conversion
	if( defaultConversionFrom_ )
	{
		return defaultConversionFrom_( this, to, fromType, from );
	}

	return false;
}


bool MetaType::convertTo( const MetaType* toType, void* to, const void* from ) const
{
	return toType->convertFrom( to, this, from );
}


void MetaType::addConversionFrom( const std::type_info& fromType, ConversionFunc func )
{
	if( fromType != typeInfo_ )
	{
		conversionsFrom_[ &fromType ] = func;
	}
}


void MetaType::setDefaultConversionFrom( ConversionFunc func )
{
	defaultConversionFrom_ = func;
}


