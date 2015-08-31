#include "test_objects.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "metadata/test_structure.mpp"
#include "metadata/test_polystruct.mpp"
#include "metadata/test_methods_object.mpp"

//==============================================================================
void TestObjects::initDefs( IDefinitionManager & definitionManager )
{
	REGISTER_DEFINITION( TestStructure );
	REGISTER_DEFINITION( TestInheritedStruct );
	REGISTER_DEFINITION( TestPolyStruct );
	REGISTER_DEFINITION( TestInheritedPolyStruct );
	REGISTER_DEFINITION( TestMethodsObject );
}


//=============================================================================
TestStructure & TestObjects::getTestStructure()
{
	return structure_;
}
