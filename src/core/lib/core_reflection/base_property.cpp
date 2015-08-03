#include "base_property.hpp"
#include "metadata/meta_base.hpp"
#include <stddef.h>

//==============================================================================
BaseProperty::BaseProperty( const char * name, const TypeId & type )
	: name_( name )
	, type_( type )
{
}


//==============================================================================
const TypeId & BaseProperty::getType() const
{
	return type_;
}


//==============================================================================
const char * BaseProperty::getName() const
{
	return name_;
}


//==============================================================================
const MetaBase * BaseProperty::getMetaData() const
{
	return nullptr;
}


//==============================================================================
void BaseProperty::setType( const TypeId & type )
{
	type_ = type;
}


//==============================================================================
void BaseProperty::setName( const char * name )
{
	name_ = name;
}