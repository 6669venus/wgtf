#include "type_id.hpp"
#include "wg_types/hash_utilities.hpp"

//==============================================================================
TypeId::TypeId( const char * name )
	: name_( name )
	, hashCode_( name != nullptr ? HashUtilities::compute( name ) : 0 )
{
}


//==============================================================================
TypeId::TypeId( const char * name, uint64_t hashCode )
	: name_( name )
	, hashCode_( hashCode )
{
}


//==============================================================================
bool TypeId::operator == ( const TypeId & other ) const
{
	return hashCode_ == other.getHashcode();
}


//==============================================================================
bool TypeId::operator != ( const TypeId & other ) const
{
	return hashCode_ != other.getHashcode();
}


//==============================================================================
bool TypeId::operator < (const TypeId & other ) const
{
	return hashCode_ < other.getHashcode();
}