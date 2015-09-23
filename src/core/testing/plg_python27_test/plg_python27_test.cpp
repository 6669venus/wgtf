#include "core_generic_plugin/generic_plugin.hpp"

#include "core_dependency_system/di_ref.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
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


	int startApplication()
	{
		DIRef< IPythonScriptingEngine > scriptingEngine( contextManager_ );
		if (scriptingEngine.get() == nullptr)
		{
			return 1;
		}

		// Import a builtin module
		{
			const bool success = scriptingEngine->import( "sys" );
			if (!success)
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

			success = scriptingEngine->import( "test" );
			if (!success)
			{
				NGT_ERROR_MSG( "Python failed to import test script.\n" );
				return 1;
			}
		}

		NGT_TRACE_MSG( "Python test successful\n" );
		return 0;
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

