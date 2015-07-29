#include "generic_plugin/generic_plugin.hpp"
#include "generic_plugin/interfaces/i_component_context.hpp"

class ControlsPlugin
	: public PluginMain
{
public:

	ControlsPlugin(IComponentContext& contextManager)
	{
	}

	bool PostLoad(IComponentContext& contextManager)
	{
		return true;
	}

	void Initialise(IComponentContext& contextManager)
	{
	}

	bool Finalise(IComponentContext& contextManager)
	{
		return true;
	}

	void Unload(IComponentContext& contextManager)
	{
	}
};

PLG_CALLBACK_FUNC( ControlsPlugin )
