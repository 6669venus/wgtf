#include "generic_plugin/generic_plugin.hpp"

#include "generic_plugin/interfaces/i_context_manager.hpp"

#include "perforce_version_control.hpp"

class PerforcePlugin
	: public PluginMain
{
public:
	PerforcePlugin(IContextManager & contextManager){}

	bool PostLoad(IContextManager & contextManager) override
	{
		types_.emplace_back(contextManager.registerInterface<PerforceVersionControl>(new PerforceVersionControl()));
		return true;
	}

	void Initialise(IContextManager & contextManager) override
	{
	}

	bool Finalise(IContextManager & contextManager) override
	{
		return true;
	}

	void Unload(IContextManager & contextManager) override
	{
		for (auto type : types_)
		{
			contextManager.deregisterInterface(type);
		}
	}

private:
	std::vector< IInterface * > types_;
};

PLG_CALLBACK_FUNC(PerforcePlugin)

