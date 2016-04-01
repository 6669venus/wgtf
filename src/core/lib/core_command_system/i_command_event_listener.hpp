#ifndef I_COMMAND_EVENT_LISTENER_HPP
#define I_COMMAND_EVENT_LISTENER_HPP

#include <memory>

class CommandInstance;

class ICommandEventListener
{
public:
	enum MultiCommandStatus
	{
		MultiCommandStatus_Begin,
		MultiCommandStatus_Cancel,
		MultiCommandStatus_Complete,
	};

	virtual void statusChanged(
		const CommandInstance & commandInstance ) const { }
	virtual void progressMade( const CommandInstance & commandInstance ) const { }
    virtual void commandExecuted( const CommandInstance & commandInstance, bool isRedoDirection ) {}
	virtual void multiCommandStatusChanged( MultiCommandStatus multiCommandStatus ) const { }
	virtual void handleCommandQueued( const char * commandId ) const { }
	virtual void onNonBlockingProcessExecution( const char * commandId ) const { }
};

#endif //I_COMMAND_EVENT_LISTENER_HPP