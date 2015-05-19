#include "automation/interfaces/automation_interface.hpp"

#include "cstdmf/cstdmf_init.hpp"
#include "cstdmf/debug_filter.hpp"
#include "cstdmf/stack_tracker.hpp"
#include "cstdmf/timestamp.hpp"
#include "cstdmf/watcher.hpp"

#include "generic_plugin_system/generic_plugin.hpp"

class Automation : public Implements< AutomationInterface >
{
public:
	Automation()
		: startTime_( BW::timestamp() )
	{
	}

	virtual bool timedOut() override
	{
		// TODO use a different flag
		if (!BW::CStdMf::checkUnattended())
		{
			return false;
		}
		const BW::uint64 currentTime = BW::timestamp();
		const double dTime = static_cast< double >( currentTime - startTime_ );
		const double dTimeS = dTime / BW::stampsPerSecondD();
		// TODO make this configurable
		const double TIMEOUT_SECONDS = 30.0;
		return (dTimeS > TIMEOUT_SECONDS);
	}

private:
	const BW::uint64 startTime_;
};

class AutomationPlugin
	: public PluginMain
{
public:
	AutomationPlugin( IContextManager & contextManager )
	{
	}

	virtual bool PostLoad( IContextManager & contextManager ) override
	{
		contextManager.registerInterface( new Automation() );
		return true;
	}
};

PLG_CALLBACK_FUNC( AutomationPlugin )

