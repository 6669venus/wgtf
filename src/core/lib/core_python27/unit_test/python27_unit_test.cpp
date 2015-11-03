#include "pch.hpp"

#include "test_fixture.hpp"
#include "core_python_script/i_module.hpp"
#include "core_python_script/i_scripting_engine.hpp"

#include <memory>


TEST( PythonReflection )
{
	TestFixture setup( m_name, result_ );

	IPythonScriptingEngine& scriptingEngine = setup.scriptingEngine_;

	// Import a builtin module
	{
		std::shared_ptr<IPythonModule> module = scriptingEngine.import( "sys" );
		CHECK( module != nullptr );
		//Python test failed to import sys
	}

	// Import a test module
	{
		const wchar_t* path = L"..\\..\\..\\src\\core\\lib\\core_python27\\unit_test\\scripts";
		bool success = scriptingEngine.appendPath( path );
		CHECK( success );
		//Python failed to set path to test script.

		std::shared_ptr<IPythonModule> module = scriptingEngine.import( "python27_test" );
		CHECK( module != nullptr );
		//Python failed to import test script.

		if (module == nullptr)
		{
			return;
		}

		success = module->callMethod( "run" );
		CHECK( success );
		//Python failed to run test script.
	}
};
