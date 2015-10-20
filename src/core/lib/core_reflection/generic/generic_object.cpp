#include "generic_object.hpp"

#include "core_reflection/generic/generic_definition.hpp"
#include "core_reflection/interfaces/i_class_definition_modifier.hpp"
#include "core_variant/variant.hpp"

#include "generic_object.mpp"

GenericObjectPtr GenericObject::create(
	IDefinitionManager & definitionManager, 
	const RefObjectId & id, 
	const char* classDefinitionName )
{
	auto defDetails = 
		definitionManager.createGenericDefinition( classDefinitionName );
	auto definition = definitionManager.registerDefinition( defDetails );
	return safeCast< GenericObject >( definition->createManagedObject( id ) );
}


//------------------------------------------------------------------------------
Variant GenericObject::getProperty( const char * name ) const
{
	const IClassDefinition & definition = this->getDefinition();
	ObjectHandle provider( this, &definition );
	PropertyAccessor accessor = definition.bindProperty( name, provider );
	if (!accessor.isValid())
	{
		assert( !"Cant get value!" );
		return Variant();
	}
	// TODO NGT-1255 this cast is not safe
	GenericProperty * property =
		( GenericProperty * ) accessor.getProperty();
	return property->get( provider, *definition.getDefinitionManager() );
}


//------------------------------------------------------------------------------
void GenericObject::setProperty(
	const char * name, const TypeId & typeId, Variant & value ) const
{
	const IClassDefinition & definition = this->getDefinition();
	ObjectHandle provider( this, &definition );
	PropertyAccessor accessor = definition.bindProperty( name, provider );
	if(!accessor.isValid())
	{
		auto property = new GenericProperty( name, typeId );
		auto & details =
			static_cast< const GenericDefinition & >( definition.getDetails() );
		details.getDefinitionModifier()->addProperty(
			property, nullptr );
		accessor = definition.bindProperty( name, provider );
		assert(accessor.isValid());
	}
	accessor.setValue( value );
}