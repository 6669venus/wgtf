#include "core_generic_plugin/generic_plugin.hpp"

#include "core_dependency_system/di_ref.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_python_script/i_scripting_engine.hpp"
#include "core_logging/logging.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/reflected_method_parameters.hpp"


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
		Variant::setMetaTypeManager( contextManager_.queryInterface<IMetaTypeManager>() );

		DIRef< IPythonScriptingEngine > scriptingEngine( contextManager_ );
		if (scriptingEngine.get() == nullptr)
		{
			return 1;
		}

		// Import a builtin module
		{
			ObjectHandle module = scriptingEngine->import( "sys" );

			if (!module.isValid())
			{
				NGT_ERROR_MSG( "Python test failed to import sys\n" );
				return 1;
			}
		}

		// Import a test module
		{
			const wchar_t* path = L"..\\..\\..\\src\\core\\testing\\plg_python27_interface_test\\scripts";
			bool success = scriptingEngine->appendPath( path );
			if (!success)
			{
				NGT_ERROR_MSG( "Python failed to set path to test script.\n" );
				return 1;
			}

			ObjectHandle module = scriptingEngine->import( "python27_test" );

			if (!module.isValid())
			{
				NGT_ERROR_MSG( "Python failed to import test script.\n" );
				return 1;
			}

			DIRef<IDefinitionManager> definitionManager( contextManager_ );

			if (definitionManager.get() == nullptr)
			{
				NGT_ERROR_MSG( "DefinitionManager not found.\n" );
				return 1;
			}

			auto moduleDefinition = module.getDefinition( *definitionManager );
			ReflectedMethodParameters parameters;
			Variant result = moduleDefinition->bindProperty( "run", module ).invoke( parameters );
			success = !result.isVoid() && !scriptingEngine->checkErrors();

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


	void Unload( IComponentContext & contextManager ) override
	{
	}
};

PLG_CALLBACK_FUNC( Python27TestPlugin )

