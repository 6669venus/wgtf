#include "pch.hpp"
#include "core_generic_plugin/generic_plugin.hpp"

#include "main_application.hpp"

#include "core_dependency_system/di_ref.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_python_script/i_scripting_engine.hpp"
#include "core_logging/logging.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/reflected_method_parameters.hpp"


class Python27TestPlugin
	: public PluginMain
{
public:
	Python27TestPlugin( IComponentContext & contextManager )
	{
	}


	bool PostLoad( IComponentContext & contextManager ) override
	{
		contextManager.registerInterface( new MainApplication( contextManager ) );
		return true;
	}


	void Initialise( IComponentContext & contextManager ) override
	{
		Variant::setMetaTypeManager( contextManager.queryInterface< IMetaTypeManager >() );
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

