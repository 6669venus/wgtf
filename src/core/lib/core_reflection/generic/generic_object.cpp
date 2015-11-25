#include "generic_object.hpp"

#include "core_reflection/generic/generic_definition.hpp"
#include "core_reflection/generic/generic_property.hpp"
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


size_t GenericObject::getPropertyCount() const
{
	auto & details = static_cast< const GenericDefinition & >(
		this->getDefinition().getDetails() );

	return details.getDefinitionModifier()->getPropertyCount();
}


IBaseProperty * GenericObject::addProperty( const char * name,
	const TypeId & typeId,
	const MetaBase * pMetaBase )
{
	auto pProperty = new GenericProperty( name, typeId );
	auto & details = static_cast< const GenericDefinition & >(
		this->getDefinition().getDetails() );
	details.getDefinitionModifier()->addProperty( pProperty, pMetaBase );
	return pProperty;
}


ObjectHandle GenericObject::getDerivedType() const
{
	// MUST pass this as a pointer and NOT (*this) as a reference or
	// ObjectHandleT will make a copy
	return ObjectHandleT< GenericObject >( this,
		&this->getDefinition() );
}


ObjectHandle GenericObject::getDerivedType()
{
	// MUST pass this as a pointer and NOT (*this) as a reference or
	// ObjectHandleT will make a copy
	return ObjectHandleT< GenericObject >( this,
		&this->getDefinition() );
}
