#include "pch.hpp"

#include "test_fixture.hpp"
#include "core_python_script/i_scripting_engine.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/reflected_method_parameters.hpp"

#include <memory>


TEST( PythonReflection )
{
	TestFixture setup( m_name, result_ );

	IPythonScriptingEngine& scriptingEngine = setup.scriptingEngine();

	// Import a builtin module
	{
		ObjectHandle module = scriptingEngine.import( "sys" );
		CHECK( module.isValid() );
		//Python test failed to import sys
	}

	// Import a test module
	{
		const wchar_t* path = L"..\\..\\..\\src\\core\\lib\\core_python27\\unit_test\\scripts";
		bool success = scriptingEngine.appendPath( path );
		CHECK( success );
		//Python failed to set path to test script.

		ObjectHandle module = scriptingEngine.import( "python27_test" );
		CHECK( module.isValid() );
		//Python failed to import test script.

		if (!module.isValid())
		{
			return;
		}

		auto contextManager = &setup.context();
		auto definitionManager = contextManager->queryInterface<IDefinitionManager>();
		CHECK( definitionManager != nullptr );

		if (definitionManager == nullptr )
		{
			return;
		}

		auto moduleDefinition = module.getDefinition( *definitionManager );
		ReflectedMethodParameters parameters;
		PropertyAccessor accessor = moduleDefinition->bindProperty( "run", module );
		CHECK( accessor.isValid() );

		Variant result = accessor.invoke( parameters );
		success = !result.isVoid();
		CHECK( success );

		success = !scriptingEngine.checkErrors();
		CHECK( success );
		//Python failed to run test script.
	}
};
