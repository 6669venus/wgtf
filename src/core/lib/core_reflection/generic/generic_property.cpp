#include "generic_property.hpp"
#include "generic_object.hpp"
#include "variant/variant.hpp"


const char * GenericProperty::getName() const
{
	return propertyName_.c_str();
}

bool GenericProperty::set( const ObjectHandle & pBase, const Variant & value ) const 
{
	auto pObject = pBase.getBase< GenericObject >();
	pObject->properties_[this] = value;
	return true;
}

Variant GenericProperty::get( const ObjectHandle & pBase ) const 
{
	auto pObject = pBase.getBase< GenericObject >();
	return pObject->properties_[this];
}