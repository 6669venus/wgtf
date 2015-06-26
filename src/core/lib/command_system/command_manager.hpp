#ifndef COMMAND_MANAGER_HPP
#define COMMAND_MANAGER_HPP

#include "command_instance.hpp"
#include "command_system_provider.hpp"
class IDefinitionManager;

namespace
{
	class BeginBatchCommand;
	class EndBatchCommand;
	class AbortBatchCommand;
}

class CommandManager
	: public Implements< ICommandManager >
{
public:
	CommandManager( const IDefinitionManager & defManager );
	~CommandManager();

	void init();
	void fini();

	//From CommandSystemProvider begin
	void registerCommand( Command * command ) override;
	void deregisterCommand( const char * commandId ) override;
	Command* findCommand(
		const char * commandId ) const override;
	ObjectHandle createArguments( const char * commandId ) override;
	CommandInstancePtr queueCommand(
		const char * commandId,
		const ObjectHandle & arguments = ObjectHandle() ) override;
	CommandInstancePtr executeCommand(
		const char * commandId,
		const ObjectHandle & arguments = ObjectHandle() ) override;
	void registerCommandStatusListener( ICommandEventListener * listener ) override;
	void fireCommandStatusChanged( const CommandInstance & command ) const override;
	void fireProgressMade( const CommandInstance & command ) const override;
	void undo() override;
	void redo() override;
	bool canUndo() const override;
	bool canRedo() const override;
	const GenericList & getHistory() const override;
	IValueChangeNotifier& currentIndex() override;
	const GenericList & getMacros() const override;
	void createCompoundCommand( const GenericList & commandInstanceList, const char * id = "" ) override;
	void deleteCompoundCommand( const char * id ) override;

	void beginBatchCommand() override;
	void endBatchCommand() override;
	void abortBatchCommand() override;
	void notifyBeginMultiCommand() override;
	void notifyCompleteMultiCommand() override;
	void notifyCancelMultiCommand() override;
	void notifyHandleCommandQueued( const char * commandId ) override;
	void notifyNonBlockingProcessExecution( const char * commandId ) override;
	bool SaveHistory( ISerializationManager & serializationMgr, IDataStream & stream ) override;
	bool LoadHistory( ISerializationManager & serializationMgr, IDataStream & stream ) override;
	NGTCommandErrorCode getLastError() const override;
	//From CommandSystemProvider end

	const IDefinitionManager & getDefManager() const;
	

private:
	friend BeginBatchCommand;
	friend EndBatchCommand;
	friend AbortBatchCommand;
	void setErrorCode( NGTCommandErrorCode errorCode ) override;
	CommandInstancePtr getActiveInstance();
	void pushActiveInstance();
	void popActiveInstance();
	void addToHistory( const CommandInstancePtr & instance );
	class CommandManagerImpl * pImpl_;
	const IDefinitionManager & defManager_;
};


class CommandManagerEventListener
	: public ICommandEventListener
{
public:
	void setCommandSystemProvider(
		ICommandManager * commandSystemProvider )
	{
		commandSystemProvider_ = commandSystemProvider;
	}

private:
	ICommandManager * commandSystemProvider_;

	void statusChanged( const CommandInstance & commandInstance ) const
	{
		commandSystemProvider_->fireCommandStatusChanged( commandInstance );
	}

	void progressMade( const CommandInstance & commandInstance ) const
	{
		commandSystemProvider_->fireProgressMade( commandInstance );
	}
};

#endif //COMMAND_MANAGER_HPP