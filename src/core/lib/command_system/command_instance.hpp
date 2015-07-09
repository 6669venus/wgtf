#ifndef COMMAND_INSTANCE_HPP
#define COMMAND_INSTANCE_HPP

#include "serialization/resizing_memory_stream.hpp"

#include "reflection/reflected_object.hpp"


#include <thread>
#include <mutex>
#include "ngt_core_common/wg_condition_variable.hpp"


namespace
{
	class CommandManagerImpl;
	struct ReflectionPropertyUndoRedoHelper;
}
class Command;
class ICommandManager;
enum class CommandErrorCode : uint8_t;

typedef std::vector< ReflectionPropertyUndoRedoHelper > UndoRedoHelperList;
enum ExecutionStatus
{
	Queued,
	Running,
	Complete
};

class CommandStatusHandler
{
public:
	virtual void setStatus( ExecutionStatus status ) = 0;
};

class CommandInstance;
typedef ObjectHandleT< CommandInstance > CommandInstancePtr;

//TODO: Pull out interface to remove linkage
/**
 *	CommandInstance stores per-instance data for a type of Command.
 */
class CommandInstance
	: public ReflectedPolyStruct
	, public CommandStatusHandler
{
	DECLARE_REFLECTED

public:
	friend CommandManagerImpl;

	CommandInstance();
	CommandInstance( const CommandInstance& );
	virtual ~CommandInstance();
	
	virtual void init( const std::thread::id& commandThreadId );

	void cancel();

	void execute();
	bool isComplete() const { return status_ == Complete; }

	ExecutionStatus getExecutionStatus() const { return status_; }
	const ObjectHandle & getArguments() const { return arguments_; }
	ObjectHandle waitForCompletion();

	CommandErrorCode getErrorCode() const;

	void addChild( const CommandInstancePtr & instance );
	bool isMultiCommand() const;

	ObjectHandle createDisplayData() const;
	void undo();
	void redo();

	bool isUndoRedoSuccessful() const;

	const IDataStream & getUndoStream() const { return undoData_; }
	const IDataStream & getRedoStream() const { return redoData_; }

	const char * getCommandId() const;
	void setContextObject( const ObjectHandle & contextObject );

	void setCommandSystemProvider( ICommandManager * pCmdSysProvider );

	ICommandManager * getCommandSystemProvider() { return pCmdSysProvider_; }

	static const char * getUndoStreamHeaderTag();
	static const char * getRedoStreamHeaderTag();
	static const char * getPropertyHeaderTag();

private:

	void getUndoData( std::string * undoData ) const;
	void setUndoData( const std::string & undoData );
	void getRedoData( std::string * undoData ) const;
	void setRedoData( const std::string & undoData );

	Command * getCommand();
	const Command * getCommand() const;
	const wchar_t* displayName() const;

	void setStatus( ExecutionStatus status );
	void setArguments( const ObjectHandle & arguments );
	void setCommandId( const char * commandName );

	void connectEvent();
	void disconnectEvent();

	void saveUndoRedoData( IDataStream & stream, const ReflectionPropertyUndoRedoHelper& helper, bool undoData = true );

	std::mutex					mutex_;
	ExecutionStatus				status_;
	wg_condition_variable		completeStatus_; // assumed predicate: status_ == Complete
	ObjectHandle				arguments_;
	ObjectHandle				returnValue_;
	std::vector< CommandInstancePtr > children_;
	ResizingMemoryStream		undoData_;
	ResizingMemoryStream		redoData_;
	ICommandManager *		pCmdSysProvider_;
	std::shared_ptr< PropertyAccessorListener > paListener_;
	UndoRedoHelperList	undoRedoHelperList_;
	std::string commandId_;
	bool						bUndoRedoSuccess_;
	ObjectHandle				contextObject_;
	CommandErrorCode			errorCode_;
};

#endif //COMMAND_INSTANCE_HPP