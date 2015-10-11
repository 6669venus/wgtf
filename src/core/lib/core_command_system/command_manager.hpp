#ifndef COMMAND_MANAGER_HPP
#define COMMAND_MANAGER_HPP

#include "command_instance.hpp"
#include "i_command_manager.hpp"
#include "wg_types/event.hpp"

class IApplication;
class IDefinitionManager;
class BatchCommand;
class UndoRedoCommand;

class CommandManager
	: public Implements< ICommandManager >
{
public:
	CommandManager( const IDefinitionManager & defManager );
	virtual ~CommandManager();

	void init( IApplication & application );
	void fini() override;

	//From ICommandManager begin
	void registerCommand( Command * command ) override;
	void deregisterCommand( const char * commandId ) override;
	Command* findCommand(
		const char * commandId ) const override;
	CommandInstancePtr queueCommand(
		const char * commandId,
		const ObjectHandle & arguments = ObjectHandle() ) override;
	void waitForInstance( const CommandInstancePtr & instance ) override;

	void registerCommandStatusListener( ICommandEventListener * listener ) override;
	void fireCommandStatusChanged( const CommandInstance & command ) const override;
	void fireProgressMade( const CommandInstance & command ) const override;
	void undo() override;
	void redo() override;
	bool canUndo() const override;
	bool canRedo() const override;
	const VariantList & getHistory() const override;
	IValueChangeNotifier& currentIndex() override;
	const IListModel & getMacros() const override;
	bool createMacro( const VariantList & commandInstanceList, const char * id = "" ) override;
	bool deleteMacroByName( const char * id ) override;

	void beginBatchCommand() override;
	void endBatchCommand() override;
	void abortBatchCommand() override;
	void notifyBeginMultiCommand() override;
	void notifyCompleteMultiCommand() override;
	void notifyCancelMultiCommand() override;
	void notifyHandleCommandQueued( const char * commandId ) override;
	void notifyNonBlockingProcessExecution( const char * commandId ) override;
	bool SaveHistory( ISerializer & serializer ) override;
	bool LoadHistory( ISerializer & serializer ) override;
	//From ICommandManager end

	const IDefinitionManager & getDefManager() const;

private:
	friend UndoRedoCommand;
	void addToHistory( const CommandInstancePtr & instance );
	bool undoRedo( const int & desiredIndex );
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