#include "pch.hpp"
#include "property.hpp"

Property::Property( const char* name )
	// TODO NGT-1161
	// At the moment I'm only storing the name of the variable.
	// Need to implement storage of values and types.
	: BaseProperty( name, TypeId( "Python" ) )
{
}
