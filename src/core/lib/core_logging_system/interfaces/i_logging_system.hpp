#ifndef I_LOGGING_SYSTEM_HPP
#define I_LOGGING_SYSTEM_HPP

// TODO: move log_level to interface folder
#include "../log_level.hpp"

class AlertManager;
class ILogger;
class LogMessage;

class ILoggingSystem
{
public:

	ILoggingSystem() {}
	virtual ~ILoggingSystem() {}

	virtual bool registerLogger( ILogger* logger ) = 0;
	virtual bool unregisterLogger( ILogger* logger ) = 0;
	virtual AlertManager* getAlertManager() = 0;
	virtual void enableAlertManagement() = 0;
	virtual void disableAlertManagement() = 0;
	virtual void log( LogLevel level, const char* format, ... ) = 0;
	virtual void log( LogMessage* message ) = 0;
	virtual void shutdown() = 0;
	virtual void process() = 0;
};

#endif // I_LOGGING_SYSTEM_HPP