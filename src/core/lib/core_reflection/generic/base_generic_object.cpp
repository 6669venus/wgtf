#include "base_generic_object.hpp"

#include "core_reflection/generic/generic_definition.hpp"
#include "core_reflection/generic/generic_property.hpp"
#include "core_reflection/interfaces/i_class_definition_modifier.hpp"
#include "core_reflection/property_accessor.hpp"

#include "base_generic_object.mpp"


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
	PropertyAccessor accessor = this->getDefinition().bindProperty( name,
		this->getDerivedType() );
	if (!accessor.isValid())
	{
		assert( false && "Property could not be found" );
		return Variant();
	}
	// TODO NGT-1255 BasePropertyWithMetaData does not have invoke()
	return accessor.invoke( parameters );
}


Variant BaseGenericObject::getProperty( const char * name ) const
{
	PropertyAccessor accessor = this->getDefinition().bindProperty( name,
		this->getDerivedType() );
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
	PropertyAccessor accessor = this->getDefinition().bindProperty( name,
		this->getDerivedType() );
	if (!accessor.isValid())
	{
		// Property does not exist
		// Add new property and set it
		const MetaBase * pMetaBase = nullptr;
		this->addProperty( name, typeId, pMetaBase );

		accessor = this->getDefinition().bindProperty( name,
			this->getDerivedType() );
		assert( accessor.isValid() );
	}

	// Try to set the found property
	return accessor.setValue( value );
}

