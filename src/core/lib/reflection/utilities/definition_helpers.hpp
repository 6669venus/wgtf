#ifndef DEFINITION_HELPERS_HPP
#define DEFINITION_HELPERS_HPP

#include <stddef.h>

class IClassDefinition;
class MetaBase;
class ReflectedPolyStruct;

//==============================================================================
template< typename T, bool IsAbstract >
struct CreateHelper
{
};


//==============================================================================
template< typename T >
struct CreateHelper< T, true >
{
	static T * create()
	{
		return NULL;
	}
};


//==============================================================================
template< typename T >
struct CreateHelper< T, false >
{
	static T * create()
	{
		return new T();
	}
};


//==============================================================================
template< typename T >
ReflectedPolyStruct * createPolyStructHelper( ReflectedPolyStruct * )
{
	return CreateHelper< T, std::is_abstract< T >::value >::create();
}


//==============================================================================
template< typename T >
ReflectedPolyStruct * createPolyStructHelper( void * )
{
	return NULL;
}


//==============================================================================
class PolyStructDefinitionSetter
{
public:
	PolyStructDefinitionSetter( ReflectedPolyStruct *, const IClassDefinition * );
	PolyStructDefinitionSetter( void *, const IClassDefinition * );
};


#endif