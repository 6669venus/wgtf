#include "core_generic_plugin/generic_plugin.hpp"

#include "core_dependency_system/di_ref.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_python_script/i_module.hpp"
#include "core_python_script/i_scripting_engine.hpp"
#include "core_logging/logging.hpp"


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
		DIRef< IPythonScriptingEngine > scriptingEngine( contextManager_ );
		if (scriptingEngine.get() == nullptr)
		{
			return 1;
		}

		// Import a builtin module
		{
			std::shared_ptr< IPythonModule > module =
				scriptingEngine->import( "sys" );
			if (module == nullptr)
			{
				NGT_ERROR_MSG( "Python test failed to import sys\n" );
				return 1;
			}
		}

		// Import a test module
		{
			const wchar_t* path = 
				L"..\\..\\..\\src\\core\\testing\\plg_python27_test\\scripts";
			bool success = scriptingEngine->appendPath( path );
			if (!success)
			{
				NGT_ERROR_MSG( "Python failed to set path to test script.\n" );
				return 1;
			}

			std::shared_ptr< IPythonModule > module =
				scriptingEngine->import( "python27_test" );
			if (module == nullptr)
			{
				NGT_ERROR_MSG( "Python failed to import test script.\n" );
				return 1;
			}

			success = module->callMethod( "run" );
			if (!success)
			{
				NGT_ERROR_MSG( "Python failed to run test script.\n" );
				return 1;
			}
		}

		NGT_TRACE_MSG( "Python test successful\n" );
		return 0;
	}

	void quitApplication() override
	{

	}

private:
	IComponentContext& contextManager_;
};


class Python27TestPlugin
	: public PluginMain
{
public:
	Python27TestPlugin( IComponentContext & contextManager )
	{
	}


	bool PostLoad( IComponentContext & contextManager ) override
	{
		contextManager.registerInterface(
			new MainApplication( contextManager ) );
		return true;
	}


	void Initialise( IComponentContext & contextManager ) override
	{
	}


	bool Finalise( IComponentContext & contextManager ) override
	{
		return true;
	}


	void Unload( IComponentContext & contextManager )
	{
	}
};

PLG_CALLBACK_FUNC( Python27TestPlugin )

