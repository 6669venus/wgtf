#include "logging_system.hpp"
#include "log_message.hpp"
#include "interfaces/i_logger.hpp"
#include "alerts/alert_manager.hpp"
#include "alerts/basic_alert_logger.hpp"
#include <cstdarg>
#include <cstdio>

LoggingSystem::LoggingSystem()
	: running_( true )
	, alertManager_( new AlertManager() )
	, basicAlertLogger_( nullptr )
	, hasAlertManagement_( false )
{
	processor_ = new std::thread( &LoggingSystem::process, this );
}

LoggingSystem::~LoggingSystem()
{
	// Kill specialized alert management and presentation
	disableAlertManagement();
	if (alertManager_ != nullptr)
	{
		delete alertManager_;
		alertManager_ = nullptr;
	}
}

void LoggingSystem::enableAlertManagement()
{
	if (!hasAlertManagement_)
	{
		basicAlertLogger_ = new BasicAlertLogger( *alertManager_ );
		registerLogger( basicAlertLogger_ );
		hasAlertManagement_ = true;
	}
}

void LoggingSystem::disableAlertManagement()
{
	if (basicAlertLogger_ != nullptr)
	{
		unregisterLogger( basicAlertLogger_ );
		delete basicAlertLogger_;
		basicAlertLogger_ = nullptr;
		hasAlertManagement_ = false;
	}
}

void LoggingSystem::shutdown()
{
	if (processor_ != nullptr)
	{
		while (!messages_.empty())
		{
			// Spin and wait until all messages have been handled by
			// the processor.
			// TODO - possibly add a sleep() here, but find a way to do 
			// this in a multi-platform way
		}

		running_ = false;
		processor_->join();
	}
	delete processor_;
}

bool LoggingSystem::registerLogger( ILogger* logger )
{
	std::lock_guard< std::mutex > guard( loggerMutex_ );

	if (std::find( loggers_.begin(), loggers_.end(), logger ) != loggers_.end())
	{
		// Logger already registered
		return false;
	}

	loggers_.push_back( logger );

	return true;
}

bool LoggingSystem::unregisterLogger( ILogger* logger )
{
	std::lock_guard< std::mutex > guard( loggerMutex_ );

	std::vector< ILogger* >::iterator itrLogger = std::find( loggers_.begin(), 
		loggers_.end(), logger );

	if (itrLogger != loggers_.end())
	{
		loggers_.erase( itrLogger );
		return true;
	}
	
	// Logger not found
	return false;
}

void LoggingSystem::log( LogLevel level, const char* format, ... )
{	
	va_list arguments;
	va_start( arguments, format );
	LogMessage* message = new LogMessage( level, format, arguments );
	va_end( arguments );

	log( message );
}

void LoggingSystem::log( LogMessage* message )
{
	std::lock_guard< std::mutex > guard( messageMutex_ );
	messages_.push( message );
}

void LoggingSystem::process()
{
	while (running_)
	{
		messageMutex_.lock();
		bool isEmpty = messages_.empty();
		messageMutex_.unlock();

		if (!isEmpty)
		{
			// Pop a message and broadcast it
			messageMutex_.lock();
			LogMessage* currentMessage = messages_.front();
			messages_.pop();
			messageMutex_.unlock();

			if (currentMessage != nullptr)
			{
				loggerMutex_.lock();
				tLoggerList::iterator itrLogger = loggers_.begin();
				tLoggerList::iterator itrLoggerEnd = loggers_.end();
				for (; itrLogger != itrLoggerEnd; ++itrLogger)
				{
					ILogger* logger = ( *itrLogger );
					if (logger != nullptr)
					{
						logger->out( currentMessage );
					}
				}
				loggerMutex_.unlock();

				delete currentMessage;
				currentMessage = nullptr;
			}
		}
	}
}