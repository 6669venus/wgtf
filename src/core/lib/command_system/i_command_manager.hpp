#ifndef I_COMMAND_MANAGER_HPP
#define I_COMMAND_MANAGER_HPP

#include "command.hpp"
#include "command_instance.hpp"
#include "i_command_event_listener.hpp"

class IValueChangeNotifier;
class GenericList;
class ISerializationManager;

enum NGTCommandErrorCode
{
	NGT_NO_ERROR = 0,
	NGT_ABORTED,
	NGT_INVALID_VALUE,
	NGT_FAILED,
} ;

class ICommandManager
{
public:
	virtual void fini() = 0;
	virtual void registerCommand( Command * command ) = 0;
	virtual void deregisterCommand( const char * commandId ) = 0;
	virtual Command * findCommand( const char * commandId ) const = 0;

	virtual CommandInstancePtr queueCommand( const char * commandId, const ObjectHandle & arguments = ObjectHandle() ) = 0;
	virtual CommandInstancePtr executeCommand( const char * commandId, const ObjectHandle & arguments = ObjectHandle() ) = 0;
	virtual void registerCommandStatusListener(
		ICommandEventListener * listener ) = 0;
	virtual void fireCommandStatusChanged(
		const CommandInstance & command ) const = 0;
	virtual void fireProgressMade( const CommandInstance & command ) const = 0;

	virtual void undo() = 0;
	virtual void redo() = 0;

	virtual bool canUndo() const = 0;
	virtual bool canRedo() const = 0;

	virtual const GenericList & getHistory() const = 0;
	virtual IValueChangeNotifier& currentIndex() = 0;
	virtual const GenericList & getMacros() const = 0;
	virtual void createCompoundCommand( const GenericList & commandInstanceList, const char * id = "" ) = 0;
	virtual void deleteCompoundCommand( const char * id ) = 0;

	
	virtual void beginBatchCommand() = 0;
	virtual void endBatchCommand() = 0;
	virtual void abortBatchCommand() = 0;

	/// Notifies for Progress Manager
	virtual void notifyBeginMultiCommand() = 0;
	virtual void notifyCompleteMultiCommand() = 0;
	virtual void notifyCancelMultiCommand() = 0;
	virtual void notifyHandleCommandQueued( const char * commandId ) = 0;
	virtual void notifyNonBlockingProcessExecution( const char * commandId ) = 0;

	virtual bool SaveHistory( ISerializationManager & serializationMgr, IDataStream & stream ) = 0;
	virtual bool LoadHistory( ISerializationManager & serializationMgr, IDataStream & stream ) = 0;

	virtual NGTCommandErrorCode getLastError() const = 0;
private:
	friend Command;
	virtual void setErrorCode( NGTCommandErrorCode errorCode ) = 0;
};

#endif//I_COMMAND_MANAGER_HPP