#include "generic_object.hpp"

#include "core_reflection/generic/generic_definition.hpp"
#include "core_reflection/generic/generic_property.hpp"
#include "core_reflection/interfaces/i_class_definition_modifier.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_variant/variant.hpp"


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


IBasePropertyPtr GenericObject::addProperty( 
	const char * name,
	const TypeId & typeId,
	MetaHandle metaData,
	Variant & value )
{
	auto definition = getDefinition();
	assert( definition != nullptr );
	auto definitionModifier = definition->getDetails().getDefinitionModifier();
	assert( definitionModifier != nullptr );

	auto definitionManager = definition->getDefinitionManager();
	assert( definitionManager != nullptr );

	auto object = getDerivedType();
	auto property = std::make_shared< GenericProperty >( name, typeId );
	property->set( object, value, *definitionManager );
	definitionModifier->addProperty( property, metaData );
	return property;
}


ObjectHandle GenericObject::getDerivedType() const
{
	// MUST pass this as a pointer and NOT (*this) as a reference or
	// ObjectHandleT will make a copy
	return ObjectHandleT< GenericObject >( this );
}


ObjectHandle GenericObject::getDerivedType()
{
	// MUST pass this as a pointer and NOT (*this) as a reference or
	// ObjectHandleT will make a copy
	return ObjectHandleT< GenericObject >( this );
}
