#ifndef I_COMMAND_MANAGER_HPP
#define I_COMMAND_MANAGER_HPP

#include "command_instance.hpp"
#include "i_command_event_listener.hpp"
#include "core_common/signal.hpp"

#include <thread>

class IValueChangeNotifier;
class VariantList;
class IListModel;
class ISerializer;
class Command;
class ISelectionContext;

class ObjectHandle;

class ISelectionContext
{
public:
	virtual const ObjectHandle & getContextObject() const = 0;
	virtual void setContextObject( const ObjectHandle & contextObject ) = 0;
};

class ICommandManager
{
	typedef Signal<void(const VariantList &, size_t, size_t)> SignalModified;
	typedef Signal<void(const VariantList &)> SignalReset;

public:
	virtual ~ICommandManager() {}
	virtual void fini() = 0;
	virtual void registerCommand( Command * command ) = 0;
	virtual void deregisterCommand( const char * commandId ) = 0;
	virtual Command * findCommand( const char * commandId ) const = 0;

	virtual CommandInstancePtr queueCommand( const char * commandId, const ObjectHandle & arguments = ObjectHandle() ) = 0;
	virtual void waitForInstance( const CommandInstancePtr & instance ) = 0;
	virtual void registerCommandStatusListener( ICommandEventListener * listener ) = 0;
	virtual void deregisterCommandStatusListener( ICommandEventListener * listener ) = 0;
	virtual void fireCommandStatusChanged(
		const CommandInstance & command ) const = 0;
	virtual void fireProgressMade( const CommandInstance & command ) const = 0;

	virtual void undo() = 0;
	virtual void redo() = 0;

	virtual bool canUndo() const = 0;
	virtual bool canRedo() const = 0;
    
    typedef std::function<bool (const CommandInstancePtr&)> TRemoveFunctor;
    virtual void removeCommands(const TRemoveFunctor & functor) = 0;

	virtual const VariantList & getHistory() const = 0;
	virtual IValueChangeNotifier& currentIndex() = 0;
	virtual const IListModel & getMacros() const = 0;
	virtual bool createMacro( const VariantList & commandInstanceList, const char * id = "" ) = 0;
	virtual bool deleteMacroByName( const char * id ) = 0;

	
	virtual void beginBatchCommand() = 0;
	virtual void endBatchCommand() = 0;
	virtual void abortBatchCommand() = 0;

	/// Notifies for Progress Manager
	virtual void notifyBeginMultiCommand() = 0;
	virtual void notifyCompleteMultiCommand() = 0;
	virtual void notifyCancelMultiCommand() = 0;
	virtual void notifyHandleCommandQueued( const char * commandId ) = 0;
	virtual void notifyNonBlockingProcessExecution( const char * commandId ) = 0;

	virtual bool SaveHistory( ISerializer & serializer ) = 0;
	virtual bool LoadHistory( ISerializer & serializer ) = 0;

	virtual ISelectionContext& selectionContext() = 0;

	virtual std::thread::id ownerThreadId() = 0;

	SignalModified signalHistoryPostInserted;
	SignalModified signalHistoryPostRemoved;
	SignalReset signalHistoryPreReset;
	SignalReset signalHistoryPostReset;
};

#endif//I_COMMAND_MANAGER_HPP
