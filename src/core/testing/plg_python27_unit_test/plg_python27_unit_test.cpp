#include "pch.hpp"
#include "core_generic_plugin/generic_plugin.hpp"

#include "core_dependency_system/di_ref.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/interfaces/i_command_line_parser.hpp"
#include "core_python_script/i_scripting_engine.hpp"
#include "core_python27/defined_instance.hpp"
#include "core_python27/scenario.hpp"
#include "core_python27/scripting_engine.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/reflected_method_parameters.hpp"
#include "core_unit_test/unit_test.hpp"

#include "reflection_test_module.hpp"


// Only ever register one
std::unique_ptr< ReflectionTestModule > reflectionModule_;
// Must be the same context with which Python27ScriptingEngine has been
// registered
IComponentContext * g_contextManager_( nullptr );

TEST( Python27 )
{
	CHECK( g_contextManager_ != nullptr );
	if (g_contextManager_ == nullptr)
	{
		return;
	}
	IComponentContext & pythonContext( *g_contextManager_ );

	DIRef< IPythonScriptingEngine > scriptingEngine( pythonContext );
	CHECK( scriptingEngine.get() != nullptr );
	if (scriptingEngine.get() == nullptr)
	{
		return;
	}

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
	reflectionModule_.reset( new ReflectionTestModule( pythonContext,
		"Python27Test",
		result_ ) );

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

		DIRef<IDefinitionManager> definitionManager( pythonContext );
		CHECK( definitionManager.get() != nullptr );
		if (definitionManager.get() == nullptr)
		{
			return;
		}

		auto moduleDefinition = module.getDefinition( *definitionManager );
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


/**
 *	Obtains a Python scripting engine interface and runs unit tests on
 *	its interface.
 */
class MainApplication
	: public Implements< IApplication >
{
public:
	MainApplication( IComponentContext& contextManager )
		: contextManager_( contextManager )
	{
	}


	int startApplication() override
	{
		g_contextManager_ = &contextManager_;

		auto clp = contextManager_.queryInterface< ICommandLineParser >();
		assert( clp != nullptr );

		return BWUnitTest::runTest( "python27_unit_test",
			clp->argc(),
			clp->argv() );
	}

	void quitApplication() override
	{
		g_contextManager_ = nullptr;
	}

private:
	IComponentContext& contextManager_;
};


/**
 *	Controls initialization and finalization of Python and
 *	registers the Python interface to be used by other plugins.
 */
class Python27UnitTestPlugin
	: public PluginMain
{
public:
	Python27UnitTestPlugin( IComponentContext & contextManager )
		: pInterface_( nullptr )
		, interpreter_( contextManager )
	{
	}


	bool PostLoad( IComponentContext & contextManager ) override
	{
		const bool transferOwnership = false;
		pInterface_ = contextManager.registerInterface(
			&interpreter_, transferOwnership );

		contextManager.registerInterface(
			new MainApplication( contextManager ) );

		return true;
	}


	void Initialise( IComponentContext & contextManager ) override
	{
		Variant::setMetaTypeManager(
			contextManager.queryInterface< IMetaTypeManager >() );

		auto pDefinitionManager_ =
			contextManager.queryInterface< IDefinitionManager >();
		if (pDefinitionManager_ == nullptr)
		{
			return;
		}

		IDefinitionManager& definitionManager = (*pDefinitionManager_);
		REGISTER_DEFINITION( ReflectedPython::DefinedInstance );
		REGISTER_DEFINITION( Scenario );

		interpreter_.init();
	}


	bool Finalise( IComponentContext & contextManager ) override
	{
		interpreter_.fini();
		return true;
	}


	void Unload( IComponentContext & contextManager )
	{
		contextManager.deregisterInterface( pInterface_ );
	}

private:
	IInterface * pInterface_;
	Python27ScriptingEngine interpreter_;
};

PLG_CALLBACK_FUNC( Python27UnitTestPlugin )

