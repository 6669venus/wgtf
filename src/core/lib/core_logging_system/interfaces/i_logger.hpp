#ifndef I_LOGGER_HPP
#define I_LOGGER_HPP

class LogMessage;

class ILogger
{
public:

	virtual void out( LogMessage* message ) = 0;
};

#endif // I_LOGGER_HPP