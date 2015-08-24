#include "variant_handler.hpp"
#include "i_definition_manager.hpp"
#include "interfaces/i_class_definition.hpp"

//==============================================================================
ReflectionStorageLookupHandler::ReflectionStorageLookupHandler(
	const IDefinitionManager * definitionManager,
	const MetaType * reflectionMetaType )
	: definitionManager_( definitionManager )
	, reflectionMetaType_( reflectionMetaType )
{
}


//==============================================================================
const MetaType * ReflectionStorageLookupHandler::findStorageType(
	const TypeId & typeId ) const 
{
	if (definitionManager_->getDefinition( typeId.getName() ))
	{
		return reflectionMetaType_;
	}
	return nullptr;
}


//==============================================================================
size_t ReflectionStorageLookupHandler::storageSize() const 
{
	return sizeof( ObjectHandle );
}


//==============================================================================
void ReflectionStorageLookupHandler::initStorage(
	void * pStorage,
	const TypeId & typeId,
	const void * value ) const 
{
	auto definition =
		definitionManager_->getDefinition( typeId.getName() );
	assert( definition );
	//Init storage
	auto providerStorage = 
		new ( pStorage ) ObjectHandle();
	//Assign to storage
	*providerStorage = definition->getBaseProvider( value );
}


//==============================================================================
bool ReflectionStorageLookupHandler::tryConvert(
	const TypeId & sourceType, const void * pSource, const TypeId & destType, void *& pDest ) const
{
	if (definitionManager_ == nullptr ||
		sourceType != TypeId::getType< ObjectHandle >())
	{
		return false;
	}
	const ObjectHandle * provider =
		static_cast< const ObjectHandle * >( pSource );
	pDest = provider->reflectedCast( destType, *definitionManager_ );
	if (pDest == nullptr)
	{
		assert( pSource == nullptr );
	}
	return true;
}
