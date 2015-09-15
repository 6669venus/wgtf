#include "object_handle_storage.hpp"
#include "reflected_object.hpp"

const IClassDefinition* getPolyStructDefinition( const ReflectedPolyStruct* polyStruct )
{
	return &polyStruct->getDefinition();
}