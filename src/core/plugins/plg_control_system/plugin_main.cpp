#include "generic_plugin_system/generic_plugin.hpp"
#include "generic_plugin_system/generic_plugin_manager.hpp"

#include "control_system/control_manager.hpp"

class ControlSystemPlugin
	: public PluginMain
{
public:
	ControlSystemPlugin( IContextManager & contextManager )
	{
	}

	void Initialise( IContextManager & contextManager ) override
	{
		Variant::setMetaTypeManager( contextManager.queryInterface< IMetaTypeManager >() );

		controlManager_.initialise();
	}

	bool Finalise( IContextManager & contextManager ) override
	{
		controlManager_.finalise();

		return true;
	}

private:
	ControlManager controlManager_;
};

PLG_CALLBACK_FUNC( ControlSystemPlugin )

