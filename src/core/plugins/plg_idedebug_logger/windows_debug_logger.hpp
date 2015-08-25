#ifndef WINDOWS_DEBUG_LOGGER_HPP
#define WINDOWS_DEBUG_LOGGER_HPP

#include "core_logging_system/interfaces/i_logger.hpp"

/*
	The WindowsDebugLogger is an ILogger implementation, which will leverage
	Windows-based output logging in Visual Studio.
 */

class WindowsDebugLogger : public ILogger
{
public:

	virtual void out( LogMessage* message );
};

#endif // WINDOWS_DEBUG_LOGGER_HPP