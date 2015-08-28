#include "object_handle_storage.hpp"
#include "reflected_object.hpp"
#include "object_handle.hpp"

const IClassDefinition* getPolyStructDefinition( const ReflectedPolyStruct* polyStruct )
{
	return &polyStruct->getDefinition();
}

void throwBaseProvider( const IClassDefinition* definition, const void* pointer )
{
	ObjectHandle baseProvider = definition->getBaseProvider( pointer );
	baseProvider.getStorage()->throwBase();
}

void* castWithBaseProvider( const IClassDefinition* definition, const void* pointer, const TypeId & typeId )
{
	ObjectHandle baseProvider = definition->getBaseProvider( pointer );
	return baseProvider.getStorage()->castHelper( typeId );
}

void* castWithProvider( const void* pointer, const TypeId & typeId )
{
	// TODO: resolve circular dependency
	return nullptr;
	//ObjectHandle provider( *pointer );
	//return provider.getStorage()->castHelper( typeId );
}