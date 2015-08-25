#include "definition_helpers.hpp"

#include "../interfaces/i_class_definition.hpp"
#include "../reflected_object.hpp"

//------------------------------------------------------------------------------
PolyStructDefinitionSetter::PolyStructDefinitionSetter(
	ReflectedPolyStruct * pStruct, const IClassDefinition * definition )
{
	if (pStruct != NULL)
	{
		pStruct->setDefinition( definition );
	}
}


//------------------------------------------------------------------------------
PolyStructDefinitionSetter::PolyStructDefinitionSetter(
	void *, const IClassDefinition * )
{
}