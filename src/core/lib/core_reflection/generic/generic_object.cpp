#include "generic_object.hpp"

#include "core_reflection/generic/generic_definition.hpp"
#include "core_reflection/interfaces/i_class_definition_modifier.hpp"
#include "core_variant/variant.hpp"

GenericObjectPtr GenericObject::create(
	IDefinitionManager & definitionManager, 
	const RefObjectId & id, 
	const char* classDefinitionName )
{
	auto defDetails = 
		definitionManager.createGenericDefinition( classDefinitionName );
	auto definition = definitionManager.registerDefinition( defDetails );
	return definition->createManagedObject( id );
}


//------------------------------------------------------------------------------
Variant GenericObject::get( const char * name ) const
{
	ObjectHandle provider( *this, definition_ );
	PropertyAccessor accessor = definition_->bindProperty( name, provider );
	if (!accessor.isValid())
	{
		assert( !"Cant get value!" );
		return Variant();
	}
	GenericProperty * property =
		( GenericProperty * ) accessor.getProperty();
	return property->get( provider );
}


//------------------------------------------------------------------------------
void GenericObject::setProperty(
	const char * name, const TypeId & typeId, Variant & value ) const
{
	ObjectHandle provider( *this, definition_ );
	PropertyAccessor accessor = definition_->bindProperty( name, provider );
	if(!accessor.isValid())
	{
		auto property = new GenericProperty( name, typeId );
		auto & details =
			static_cast< const GenericDefinition & >( definition_->getDetails() );
		details.getDefinitionModifier()->addProperty(
			property, nullptr );
		accessor = definition_->bindProperty( name, provider );
		assert(accessor.isValid());
	}
	accessor.setValue( value );
}