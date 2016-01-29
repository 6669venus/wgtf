#include "core_generic_plugin/generic_plugin.hpp"
#include "core_generic_plugin_manager/generic_plugin_manager.hpp"
#include "core_logging_system/logging_system.hpp"
#include <vector>
#include <memory>

/**
 * LoggingSystemPlugin
 *
 * A plugin used to expose the core logging system for NGT.
 */

class LoggingSystemPlugin
	: public PluginMain
{
public:

	LoggingSystemPlugin( IComponentContext & contextManager )
	{
	}

	bool PostLoad( IComponentContext & contextManager ) override
	{		
		loggingSystem_ = new LoggingSystem();
		contextManager.registerInterface( loggingSystem_ );

		return true;
	}

	void Initialise( IComponentContext & contextManager ) override
	{
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
		if (loggingSystem_ != nullptr)
		{
			loggingSystem_->shutdown();
		}

		return true;
	}

	void Unload( IComponentContext & contextManager ) override
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
