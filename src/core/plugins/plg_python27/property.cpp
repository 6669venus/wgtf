#include "pch.hpp"
#include "property.hpp"

Property::Property( const char* name )
	// TODO name only, needs values and types
	: BaseProperty( name, TypeId( "Python" ) )
{
}
