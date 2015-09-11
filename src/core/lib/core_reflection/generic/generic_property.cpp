#include "generic_property.hpp"
#include "generic_object.hpp"
#include "core_variant/variant.hpp"


const char * GenericProperty::getName() const
{
	return propertyName_.c_str();
}

bool GenericProperty::set( const ObjectHandle & pBase, const Variant & value, const IDefinitionManager & definitionManager ) const 
{
	auto pObject = reflectedCast< GenericObject >( pBase, definitionManager );
	pObject->properties_[this] = value;
	return true;
}

Variant GenericProperty::get( const ObjectHandle & pBase, const IDefinitionManager & definitionManager ) const 
{
	auto pObject = reflectedCast< GenericObject >( pBase, definitionManager );
	return pObject->properties_[this];
}