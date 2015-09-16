#include "reflection_utilities.hpp"
#include "../reflected_object.hpp"
#include "../property_accessor.hpp"

namespace {

	bool canCastToPolyStruct( const IClassDefinition & classDef )
	{
		return classDef.canBeCastTo(
			*classDef.getDefinitionManager()->getDefinition< ReflectedPolyStruct >() );
	}

}

namespace ReflectionUtilities
{

// =============================================================================
bool isPolyStruct( const PropertyAccessor & pa ) 
{
	auto structDef = pa.getStructDefinition();
	if(structDef != nullptr)
	{
		return canCastToPolyStruct( *structDef );
	}
	return false;
}

// =============================================================================
bool isStruct( const PropertyAccessor & pa ) 
{
	auto structDef = pa.getStructDefinition();
	if(structDef != nullptr)
	{
		return !canCastToPolyStruct( *structDef );
	}
	return false;
}

// =============================================================================
template<>
Variant copy< Variant >( Variant & value )
{
	return value;
}

// =============================================================================
template<>
Variant copy< const Variant >( const Variant & value )
{
	return value;
}

// =============================================================================
template<>
Variant reference< Variant >( Variant & value )
{
	return value;
}

// =============================================================================
template<>
Variant reference< const Variant >( const Variant & value )
{
	return value;
}

}