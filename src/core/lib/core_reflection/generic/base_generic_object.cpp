#include "base_generic_object.hpp"

#include "core_reflection/generic/generic_definition.hpp"
#include "core_reflection/generic/generic_property.hpp"
#include "core_reflection/interfaces/i_class_definition_modifier.hpp"
#include "core_reflection/property_accessor.hpp"


BaseGenericObject::BaseGenericObject()
	: definition_( nullptr )
{

}


BaseGenericObject::~BaseGenericObject()
{

}


IClassDefinition * BaseGenericObject::getDefinition() const
{
	return definition_;
}


void BaseGenericObject::setDefinition( IClassDefinition * definition )
{
	definition_ = definition;
}


bool BaseGenericObject::set( const char * name, const Variant & value )
{
	return this->setProperty( name,
		value.type()->typeId(),
		const_cast< Variant & >( value ) );
}


Variant BaseGenericObject::invoke( const char * name,
	const ReflectedMethodParameters& parameters )
{
	return this->invokeProperty( name, parameters );
}


Variant BaseGenericObject::invokeProperty( const char * name,
	const ReflectedMethodParameters& parameters )
{
	const IClassDefinition & definition = *this->getDefinition();
	ObjectHandle provider = this->getDerivedType();
	PropertyAccessor accessor = definition.bindProperty( name, provider );
	if (!accessor.isValid())
	{
		assert( false && "Property could not be found" );
		return Variant();
	}
	// TODO NGT-1255 BasePropertyWithMetaData does not have invoke()
	return accessor.invoke( parameters );
}



PropertyAccessor BaseGenericObject::findProperty( const char * name ) const
{
	const IClassDefinition & definition = *this->getDefinition();
	ObjectHandle provider = this->getDerivedType();
	return definition.bindProperty( name, provider );
}


Variant BaseGenericObject::getProperty( const char * name ) const
{
	auto accessor = findProperty( name );
	if (!accessor.isValid())
	{
		assert( false && "Property could not be found" );
		return Variant();
	}
	return accessor.getValue();
}


bool BaseGenericObject::setProperty( const char * name,
	const TypeId & typeId,
	Variant & value )
{
	// Get existing property
	const IClassDefinition & definition = *this->getDefinition();
	ObjectHandle provider = this->getDerivedType();
	PropertyAccessor accessor = definition.bindProperty( name, provider );
	if (accessor.isValid())
	{
		return accessor.setValue( value );
	}

	// Property does not exist
	// Add new property and set it
	auto property = this->addProperty( name, typeId, nullptr, value );
	return property != nullptr;
}

