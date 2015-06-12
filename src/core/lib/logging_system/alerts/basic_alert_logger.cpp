#include "basic_alert_logger.hpp"
#include "logging_system/alerts/alert_manager.hpp"
#include "logging_system/log_message.hpp"
#include "logging_system/log_level.hpp"
#include "generic_plugin/interfaces/i_context_manager.hpp"

#include <stdio.h>

BasicAlertLogger::BasicAlertLogger( AlertManager & alertManager )
	: alertManager_( alertManager )
{
}

BasicAlertLogger::~BasicAlertLogger()
{
}

void BasicAlertLogger::out( LogMessage* message )
{
	if (message->getLevel() == LOG_ALERT)
	{
		alertManager_.add( message->c_str() );
	}
}
