#ifndef COMMAND_MANAGER_HPP
#define COMMAND_MANAGER_HPP

#include "command_instance.hpp"
#include "i_command_manager.hpp"
class IDefinitionManager;

class BatchCommand;
class UndoRedoCommand;

class CommandManager
	: public Implements< ICommandManager >
{
public:
	CommandManager( const IDefinitionManager & defManager, const std::thread::id & ownerThreadId );
	~CommandManager();

	void init();
	void fini();

	//From ICommandManager begin
	void registerCommand( Command * command ) override;
	void deregisterCommand( const char * commandId ) override;
	Command* findCommand(
		const char * commandId ) const override;
	CommandInstancePtr queueCommand(
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
	bool createMacro( const GenericList & commandInstanceList, const char * id = "" ) override;
	bool deleteMacroByName( const char * id ) override;

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
	//From ICommandManager end

	const IDefinitionManager & getDefManager() const;
	const std::thread::id & getOwnerThreadId() const;

private:
	friend BatchCommand;
	friend UndoRedoCommand;
	CommandInstancePtr getActiveInstance() const;
	void pushActiveInstance( const CommandInstancePtr & instance );
	CommandInstancePtr popActiveInstance();
	void addToHistory( const CommandInstancePtr & instance );
	bool undoRedo( const int & desiredIndex );
	class CommandManagerImpl * pImpl_;
	const IDefinitionManager & defManager_;
	const std::thread::id							ownerThreadId_;
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