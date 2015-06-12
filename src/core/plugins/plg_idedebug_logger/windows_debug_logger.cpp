#include "windows_debug_logger.hpp"
#include "logging_system/log_message.hpp"
#include <iostream>
#include <time.h>
#include <iomanip>
#include <sstream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void WindowsDebugLogger::out( LogMessage* message )
{
	if (message != nullptr)
	{
		auto currentTime = time(nullptr);
		auto tm = *localtime( &currentTime );

		std::stringstream ss;
		
		ss << "[" << message->getLevelString() << "]["
			<< std::put_time( &tm, "%d/%m/%Y %H:%M:%S" ) << "] "
			<< message->c_str()
			<< std::endl;

		OutputDebugStringA( ss.str().c_str() );
	}
}
