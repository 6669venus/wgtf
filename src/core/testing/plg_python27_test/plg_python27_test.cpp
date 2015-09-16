#include "core_generic_plugin/generic_plugin.hpp"

#include "core_dependency_system/di_ref.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_python_script/i_interpreter.hpp"
#include "core_logging/logging.hpp"


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
		DIRef< IPythonInterpreter > interpreter( contextManager_ );
		if (interpreter.get() == nullptr)
		{
			return 1;
		}

		// Import a builtin module
		{
			const bool success = interpreter->import( "sys" );
			if (!success)
			{
				NGT_ERROR_MSG( "Python test failed to import sys\n" );
				return 1;
			}
		}

		// TODO not working
		// Import a test module
		//{
		//	const bool success = interpreter->import( "Python27Test" );
		//	if (!success)
		//	{
		//		NGT_ERROR_MSG( "Python test failed to import Python27Test\n" );
		//		return 1;
		//	}
		//}

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

