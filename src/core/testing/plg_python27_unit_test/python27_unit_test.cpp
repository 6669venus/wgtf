#include "pch.hpp"

#include "core_dependency_system/di_ref.hpp"
#include "core_python27/defined_instance.hpp"
#include "core_python27/scenario.hpp"
#include "core_python27/scripting_engine.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/reflected_method_parameters.hpp"

#include "reflection_test_module.hpp"


// Must be the same context with which Python27ScriptingEngine has been
// registered
IComponentContext * g_contextManager_( nullptr );


class ScopedScriptingEngine
{
public:
	ScopedScriptingEngine( IComponentContext & contextManager )
		: scriptingEngine_( contextManager )
	{
		scriptingEngine_.init();
	}
	~ScopedScriptingEngine()
	{
		scriptingEngine_.fini();
	}
	Python27ScriptingEngine scriptingEngine_;
};


TEST( Python27 )
{
	CHECK( g_contextManager_ != nullptr );
	if (g_contextManager_ == nullptr)
	{
		return;
	}
	IComponentContext & contextManager( *g_contextManager_ );

	DIRef< IDefinitionManager > pDefinitionManager( contextManager );
	CHECK( pDefinitionManager.get() != nullptr );
	if (pDefinitionManager.get() == nullptr)
	{
		return;
	}

	IDefinitionManager& definitionManager = *pDefinitionManager.get();
	REGISTER_DEFINITION( ReflectedPython::DefinedInstance );
	REGISTER_DEFINITION( Scenario );


	ScopedScriptingEngine scopedScriptingEngine( contextManager );
	IPythonScriptingEngine * scriptingEngine =
		&scopedScriptingEngine.scriptingEngine_;

	// Import a builtin module
	{
		ObjectHandle module = scriptingEngine->import( "sys" );
		CHECK( module.isValid() );
		// Python test failed to import sys
		if (!module.isValid())
		{
			return;
		}
	}

	// Register the test module
	// The scripting engine must be shut down to de-register it.
	ReflectionTestModule reflectionModule( contextManager,
		"Python27Test",
		result_ );

	// Import a test module
	{
		const wchar_t* path =
			L"..\\..\\..\\src\\core\\testing\\plg_python27_unit_test\\scripts";
		const bool pathAppended = scriptingEngine->appendPath( path );
		CHECK( pathAppended );
		// Python failed to set path to test script.
		if (!pathAppended)
		{
			return;
		}

		ObjectHandle module = scriptingEngine->import( "python27_test" );
		CHECK( module.isValid() );
		// Python failed to import test script.
		if (!module.isValid())
		{
			return;
		}

		auto moduleDefinition = module.getDefinition( definitionManager );
		ReflectedMethodParameters parameters;
		auto propertyAccessor = moduleDefinition->bindProperty( "run", module );
		CHECK( propertyAccessor.isValid() );
		if (!propertyAccessor.isValid())
		{
			return;
		}

		auto result = propertyAccessor.invoke( parameters );
		CHECK( !result.isVoid() && !scriptingEngine->checkErrors() );
		// Python failed to run test script.
	}
}

