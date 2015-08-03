#include "core_generic_plugin/generic_plugin.hpp"

#include "core_control_system/control_manager.hpp"

class ControlSystemPlugin
	: public PluginMain
{
public:
	ControlSystemPlugin( IComponentContext & contextManager )
	{
	}

	void Initialise( IComponentContext & contextManager ) override
	{
		Variant::setMetaTypeManager( contextManager.queryInterface< IMetaTypeManager >() );

		controlManager_.initialise();
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
		controlManager_.finalise();

		return true;
	}

private:
	ControlManager controlManager_;
};

PLG_CALLBACK_FUNC( ControlSystemPlugin )
