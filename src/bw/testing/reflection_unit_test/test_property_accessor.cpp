#include "pch.hpp"
#include "unit_test_lib/unit_test.hpp"

#include "test_reflection_fixture.hpp"
#include "reflection/interfaces/i_base_property.hpp"
#include "reflection/property_accessor.hpp"

TEST_F( TestReflectionFixture, testBinding )
{
	TestStructure & testStructure = getTestStructure();

	ObjectHandle provider(
		testStructure,
		getDefinitionManager().getDefinition< TestStructure >() );

	auto definition = provider.getDefinition();
	CHECK( definition );
	auto itRange = definition->allProperties();
	for( auto it = itRange.begin(); it != itRange.end(); ++it )
	{
		auto property = *it;
		definition->bindProperty( property->getName(), provider );
	}

}