#include "generic_plugin/generic_plugin.hpp"
#include "generic_plugin_manager/generic_plugin_manager.hpp"
#include "logging_system/interfaces/i_logging_system.hpp"
#include "logging_system/log_level.hpp"
#include "windows_debug_logger.hpp"
#include <vector>

/*
	IDEDebugLoggerPlugin

	Registers a logger specific to the IDE. May be configured to support
	additional IDEs. At this time it supports Windows/Visual Studio output.
 */

class IDEDebugLoggerPlugin
	: public PluginMain
{
public:

	IDEDebugLoggerPlugin( IContextManager & contextManager )
		: windowsDebugLogger_( nullptr)
	{
	}

	bool PostLoad( IContextManager & contextManager ) override
	{		
		ILoggingSystem* loggingSystem = 
			contextManager.queryInterface< ILoggingSystem > ();
		if (loggingSystem == nullptr)
		{
			return false;
		}

		// Register the Windows debug logger
		windowsDebugLogger_ = new WindowsDebugLogger();
		loggingSystem->registerLogger( windowsDebugLogger_ );
		loggingSystem->log( LOG_INFO, 
			"Windows Debug Logger registered and ready." );

		return true;
	}

	void Initialise( IContextManager & contextManager ) override
	{
	}

	bool Finalise( IContextManager & contextManager ) override
	{
		ILoggingSystem* loggingSystem = 
			contextManager.queryInterface< ILoggingSystem > ();
		if (loggingSystem == nullptr)
		{
			return false;
		}

		loggingSystem->unregisterLogger( windowsDebugLogger_ );

		if (windowsDebugLogger_ != nullptr)
		{
			delete windowsDebugLogger_;
			windowsDebugLogger_ = nullptr;
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
	WindowsDebugLogger* windowsDebugLogger_;
};

PLG_CALLBACK_FUNC( IDEDebugLoggerPlugin )