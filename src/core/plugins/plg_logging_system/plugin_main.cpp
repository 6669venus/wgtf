#include "generic_plugin/generic_plugin.hpp"
#include "generic_plugin_manager/generic_plugin_manager.hpp"
#include "logging_system/logging_system.hpp"
#include <vector>

/**
 * LoggingSystemPlugin
 *
 * A plugin used to expose the core logging system for NGT.
 */

class LoggingSystemPlugin
	: public PluginMain
{
public:

	LoggingSystemPlugin( IContextManager & contextManager )
	{
	}

	bool PostLoad( IContextManager & contextManager ) override
	{		
		loggingSystem_ = new LoggingSystem();
		contextManager.registerInterface( loggingSystem_, false );

		return true;
	}

	void Initialise( IContextManager & contextManager ) override
	{
	}

	bool Finalise( IContextManager & contextManager ) override
	{
		if (loggingSystem_ != nullptr)
		{
			loggingSystem_->shutdown();
		}

		return true;
	}

	void Unload( IContextManager & contextManager ) override
	{
		for ( auto type: types_ )
		{
			contextManager.deregisterInterface( type );
		}
	}

private:

	std::vector< IInterface * > types_;
	LoggingSystem* loggingSystem_;
};

PLG_CALLBACK_FUNC( LoggingSystemPlugin )
