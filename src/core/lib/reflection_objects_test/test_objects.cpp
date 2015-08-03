#include "test_objects.hpp"
#include "reflection/reflection_macros.hpp"
#include "metadata/test_structure.mpp"
#include "metadata/test_polystruct.mpp"

//==============================================================================
void TestObjects::initDefs( IDefinitionManager & definitionManager )
{
	REGISTER_DEFINITION( TestStructure );
	REGISTER_DEFINITION( TestInheritedStruct );
	REGISTER_DEFINITION( TestPolyStruct );
	REGISTER_DEFINITION( TestInheritedPolyStruct );
}


//=============================================================================
TestStructure & TestObjects::getTestStructure()
{
	return structure_;
}