#include "generic_plugin/generic_plugin.hpp"
#include "generic_plugin/interfaces/i_context_manager.hpp"

class ControlsPlugin
	: public PluginMain
{
public:

	ControlsPlugin(IContextManager& contextManager)
	{
	}

	bool PostLoad(IContextManager& contextManager)
	{
		return true;
	}

	void Initialise(IContextManager& contextManager)
	{
	}

	bool Finalise(IContextManager& contextManager)
	{
		return true;
	}

	void Unload(IContextManager& contextManager)
	{
	}
};

PLG_CALLBACK_FUNC( ControlsPlugin )
