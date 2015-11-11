#include "command_manager.hpp"
#include "command_instance.hpp"
#include "compound_command.hpp"
#include "undo_redo_command.hpp"
#include "i_command_event_listener.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
//TODO: remove this pragma
#pragma warning (push)
#pragma warning (disable : 4996 )
#include "core_data_model/generic_list.hpp"
#pragma warning( pop )
#include "core_data_model/value_change_notifier.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_variant/variant.hpp"
#include "wg_types/hashed_string_ref.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_serialization/serializer/i_serializer.hpp"
#include "core_logging/logging.hpp"
#include "batch_command.hpp"
#include <atomic>
#include <deque>
#include <thread>
#include <map>
#include <mutex>
#include "core_common/wg_condition_variable.hpp"
#include "core_common/thread_local_value.hpp"

// TODO: Remove to platform string header
#if defined( _WIN32 )
#define snprintf sprintf_s
#endif

namespace
{

	struct CommandFrame
	{
		CommandFrame( const CommandInstancePtr & instance )
		{
			stackQueue_.push_back( instance );
			commandStack_.push_back( instance );
		}

		std::deque< CommandInstancePtr > stackQueue_;
		std::deque< CommandInstancePtr > commandStack_;
		std::deque< CommandInstancePtr > commandQueue_;
	};

class CommandManagerImpl
{
public:
	static const int NO_SELECTION;

	CommandManagerImpl( CommandManager* pCommandManager )
		: currentIndex_( NO_SELECTION )
		, previousSelectedIndex_( NO_SELECTION )
		, workerMutex_()
		, workerWakeUp_()
		, ownerWakeUp_( false )
		, commands_()
		, commandFrames_()
		, currentFrame_( nullptr )
		, history_()
		, macros_()
		, eventListenerCollection_()
		, globalEventListener_()
		, exiting_( false )
		, enableWorker_( true )
		, ownerThreadId_( std::this_thread::get_id() )
		, workerThreadId_()
		, pCommandManager_( pCommandManager )
		, workerThread_()
		, batchCommand_( pCommandManager )
		, undoRedoCommand_( pCommandManager )
		, application_( nullptr )
	{
		commandFrames_.push_back( new CommandFrame( nullptr ) );
		THREAD_LOCAL_SET( currentFrame_, commandFrames_.back() );
	}

	~CommandManagerImpl()
	{
		{
			// mutex lock is needed here to ensure new exiting_ value
			// is visible in other thread (due to memory barrier introduced by lock/unlock)
			std::unique_lock<std::mutex> lock( workerMutex_ );
			exiting_ = true;
			workerWakeUp_.notify_all();
		}

		if (enableWorker_)
		{
			workerThread_.join();
		}

		assert( commandFrames_.size() == 1 );
		delete commandFrames_.back();
		commandFrames_.clear();

		history_.onPostItemsRemoved().remove< CommandManagerImpl,
			&CommandManagerImpl::onPostItemsRemoved >( this );

		currentIndex_.onPreDataChanged().remove< CommandManagerImpl,
			&CommandManagerImpl::onPreDataChanged >( this );
		currentIndex_.onPostDataChanged().remove< CommandManagerImpl,
			&CommandManagerImpl::onPostDataChanged >( this );

		pCommandManager_ = nullptr;
	}

	void init( IApplication & application );
	void fini();
	void update( const IApplication * sender, const IApplication::UpdateArgs & args );
	void registerCommand( Command * command );
	void deregisterCommand( const char * commandName );
	Command * findCommand(const char * commandName ) const;
	CommandInstancePtr queueCommand(
		const char * commandName, const ObjectHandle & arguments );
	void queueCommand( const CommandInstancePtr & instance );
	void waitForInstance( const CommandInstancePtr & instance );
	void registerCommandStatusListener( ICommandEventListener * listener );
	void fireCommandStatusChanged( const CommandInstance & command ) const;
	void fireProgressMade( const CommandInstance & command ) const;
	void updateSelected( const int & value );

	void undo();
	void redo();
	bool canUndo() const;
	bool canRedo() const;
	VariantList & getHistory();
	ValueChangeNotifier< int > & getCurrentIndex();
	IListModel & getMacros();

	void beginBatchCommand();
	void endBatchCommand();
	void abortBatchCommand();
	void notifyBeginMultiCommand();
	void notifyCompleteMultiCommand();
	void notifyCancelMultiCommand();
	void notifyHandleCommandQueued( const char * commandId );
	void notifyNonBlockingProcessExecution( const char * commandId );

	void pushFrame( const CommandInstancePtr & instance );
	void popFrame();
	bool createCompoundCommand( const VariantList & commandInstanceList, const char * id );
	bool deleteCompoundCommand( const char * id );
	void addToHistory( const CommandInstancePtr & instance );
	void processCommands();
	void flush();
	bool SaveCommandHistory( ISerializer & serializer );
	bool LoadCommandHistory( ISerializer & serializer );
	void threadFunc();

	ValueChangeNotifier< int >				currentIndex_;
	int										previousSelectedIndex_;

private:
	typedef std::unordered_map< HashedStringRef, Command * > CommandCollection;
	typedef std::list< ICommandEventListener * > EventListenerCollection;

	/*
	Guard data shared with worker thread.
	Try to cover as less code as possible by mutex, but not less than required.
	Especially avoid calling user callbacks while holding it.
	*/
	std::mutex								workerMutex_;

	/*
	Assumed predicate: worker has something to do (at least one of these):
	- commandQueue_ is not empty
	- exiting_ == true
	*/
	wg_condition_variable					workerWakeUp_;
	std::atomic<bool>						ownerWakeUp_;

	CommandCollection						commands_;
	std::vector< CommandFrame * >			commandFrames_;
	THREAD_LOCAL( CommandFrame *)			currentFrame_;

	std::deque< CommandInstancePtr >		pendingHistory_;
	VariantList								history_;
	GenericListT< ObjectHandleT< CompoundCommand > > macros_;
	EventListenerCollection					eventListenerCollection_;
	std::unique_ptr< ICommandEventListener > globalEventListener_;

	bool									exiting_;
	bool									enableWorker_;
	std::thread::id							ownerThreadId_;
	std::thread::id							workerThreadId_;
	CommandManager*							pCommandManager_;
	std::thread								workerThread_;
	BatchCommand							batchCommand_;
	UndoRedoCommand							undoRedoCommand_;

	IApplication *							application_;

	void multiCommandStatusChanged( ICommandEventListener::MultiCommandStatus status );
	void onPreDataChanged( const IValueChangeNotifier* sender,
		const IValueChangeNotifier::PreDataChangedArgs& args );
	void onPostDataChanged( const IValueChangeNotifier* sender,
		const IValueChangeNotifier::PostDataChangedArgs& args );
	void onPostItemsRemoved( const IListModel* sender,
		const IListModel::PostItemsRemovedArgs& args );

	void addBatchCommandToCompoundCommand(
		const ObjectHandleT<CompoundCommand> & compoundCommand,
		const CommandInstancePtr & instance );
};

const int CommandManagerImpl::NO_SELECTION = -1;

//==============================================================================
void CommandManagerImpl::init( IApplication & application )
{
	application_ = &application;
	application_->onUpdate().add<CommandManagerImpl, &CommandManagerImpl::update>( this );

	CommandManagerEventListener *
		listener = new CommandManagerEventListener();
	listener->setCommandSystemProvider( pCommandManager_ );
	globalEventListener_.reset( listener );

	registerCommand( &batchCommand_ );
	registerCommand( &undoRedoCommand_ );

	currentIndex_.onPreDataChanged().add< CommandManagerImpl,
		&CommandManagerImpl::onPreDataChanged >( this );
	currentIndex_.onPostDataChanged().add< CommandManagerImpl,
		&CommandManagerImpl::onPostDataChanged >( this );

	history_.onPostItemsRemoved().add< CommandManagerImpl,
		&CommandManagerImpl::onPostItemsRemoved >( this );

	if (enableWorker_)
	{
		workerThread_ = std::thread( &CommandManagerImpl::threadFunc, this );
		workerThreadId_ = workerThread_.get_id();
	}
	else
	{
		workerThreadId_ = ownerThreadId_;
	}
}

//==============================================================================
void CommandManagerImpl::fini()
{
	assert( application_ != nullptr );
	application_->onUpdate().remove<CommandManagerImpl, &CommandManagerImpl::update>( this );
}

//==============================================================================
void CommandManagerImpl::update( const IApplication * sender, const IApplication::UpdateArgs & args )
{
	// Optimisation to early out before calling processCommands which will attempt to acquire a mutex
	if (!ownerWakeUp_)
	{
		return;
	}

	processCommands();

	ownerWakeUp_ = false;
}

//==============================================================================
void CommandManagerImpl::registerCommand( Command * command )
{
	auto findIt = commands_.find( command->getId() );
	if (findIt != commands_.end())
	{
		assert( false );
		return;
	}
	commands_.insert(
		std::make_pair( command->getId(), command ) );
	command->registerCommandStatusListener( globalEventListener_.get() );
	command->setCommandSystemProvider( pCommandManager_ );
}

//==============================================================================
void CommandManagerImpl::deregisterCommand( const char * commandId )
{
	auto findIt = commands_.find( commandId );
	if (findIt != commands_.end())
	{
		commands_.erase( findIt );
	}
}


//==============================================================================
Command * CommandManagerImpl::findCommand(const char * commandId ) const
{
	auto findIt = commands_.find( commandId );
	if (findIt != commands_.end())
	{
		return findIt->second;
	}
	return nullptr;
}


//==============================================================================
CommandInstancePtr CommandManagerImpl::queueCommand(
	const char * commandId, const ObjectHandle & arguments )
{
	Command * command = findCommand( commandId );
	if(command == nullptr)
	{
		NGT_ERROR_MSG( "Command %s not found. \n", commandId );
		return nullptr;
	}

	auto instance =
		pCommandManager_->getDefManager().create< CommandInstance >();
	instance->setCommandSystemProvider( pCommandManager_ );
	instance->setCommandId( command ->getId() );
	instance->setArguments( arguments );
	instance->setDefinitionManager(
		const_cast<IDefinitionManager&>(pCommandManager_->getDefManager()) );
	instance->init();
	instance->setStatus( Queued );
	queueCommand( instance );
	return instance;
}


//==============================================================================
void CommandManagerImpl::queueCommand( const CommandInstancePtr & instance )
{
	std::thread::id currentThreadId = std::this_thread::get_id();
	assert( ( currentThreadId == workerThreadId_ || currentThreadId == ownerThreadId_ )
		&& "queueCommand can only be called in command thread and owner thread. \n" );

	{
		std::unique_lock<std::mutex> lock( workerMutex_ );

		// Push the command onto the queue of the relevant command frame, determined by the current thread
		auto commandFrame = THREAD_LOCAL_GET( currentFrame_ );
		commandFrame->commandQueue_.push_back( instance );

		// If the command is a batch command we need to push/pop to the current command frames stack queue.
		// The stack queue is used to record pending batch command groups that have not been processed yet.
		if (strcmp( instance->getCommandId(), typeid( BatchCommand ).name() ) == 0)
		{
			auto stage = instance->getArguments().getBase<BatchCommandStage>();
			assert( stage != nullptr );
			if (*stage == BatchCommandStage::Begin)
			{
				commandFrame->stackQueue_.push_back( instance );
			}
			else
			{
				assert( !commandFrame->stackQueue_.empty() );
				commandFrame->stackQueue_.pop_back();
			}
		}
	}

	// Try to execute the queued commands instantly.
	// This will either execute the command or notify the appropriate thread to start processing
	processCommands();
}


//==============================================================================
void CommandManagerImpl::waitForInstance( const CommandInstancePtr & instance )
{
	// TODO: Introduce a timeout option - primarily for the unit tests

	// The stack queue (pending Batch Command groups) will tell us the actual commmand to wait for.
	// For example in the following scenario
	// 0: BeginBatchCommand
	// 1:  BeginBatchCommand
	// 2:   SetProperty-A
	// 3:  EndBatchCommand
	// 4:  WaitForCommand-2
	// 5: EndBatchCommand
	// command 4 actually needs to wait for command 1, as the result of command 2 should not be visible to
	// command 4 until the BatchCommand it belongs to has completed.

	std::deque< CommandInstancePtr > stackQueue;
	{
		std::unique_lock<std::mutex> lock( workerMutex_ );
		auto commandFrame = THREAD_LOCAL_GET( currentFrame_ );
		stackQueue = commandFrame->stackQueue_;
	}
	assert( std::find( stackQueue.begin(), stackQueue.end(), instance ) == stackQueue.end() );

	auto waitFor = instance;
	while (std::find( stackQueue.begin(), stackQueue.end(), waitFor ) == stackQueue.end())
	{
		while (waitFor->status_ != Complete)
		{
			processCommands();

			// TODO: This is piggy backing on now defunct command status messages.
			// This needs to be revised.
			fireProgressMade( *waitFor );
		}
		waitFor = waitFor->parent_;
		if (waitFor == nullptr)
		{
			break;
		}
	}
}


//==============================================================================
void CommandManagerImpl::registerCommandStatusListener(
	ICommandEventListener * listener )
{
	eventListenerCollection_.push_back( listener );
}


//==============================================================================
void CommandManagerImpl::fireCommandStatusChanged( const CommandInstance & command ) const
{
	EventListenerCollection::const_iterator it =
		eventListenerCollection_.begin();
	EventListenerCollection::const_iterator itEnd =
		eventListenerCollection_.end();
	for( ; it != itEnd; ++it )
	{
		(*it)->statusChanged( command );
	}
}


//==============================================================================
void CommandManagerImpl::fireProgressMade( const CommandInstance & command ) const
{
	EventListenerCollection::const_iterator it =
		eventListenerCollection_.begin();
	EventListenerCollection::const_iterator itEnd =
		eventListenerCollection_.end();
	for( ; it != itEnd; ++it )
	{
		(*it)->progressMade( command );
	}
}


//==============================================================================
void CommandManagerImpl::updateSelected( const int & value )
{
	currentIndex_.onPreDataChanged().remove< CommandManagerImpl,
		&CommandManagerImpl::onPreDataChanged >( this );
	currentIndex_.onPostDataChanged().remove< CommandManagerImpl,
		&CommandManagerImpl::onPostDataChanged >( this );
	currentIndex_.value( value );
	previousSelectedIndex_ = value;
	currentIndex_.onPreDataChanged().add< CommandManagerImpl,
		&CommandManagerImpl::onPreDataChanged >( this );
	currentIndex_.onPostDataChanged().add< CommandManagerImpl,
		&CommandManagerImpl::onPostDataChanged >( this );
}


//==============================================================================
bool CommandManagerImpl::canUndo() const
{
	if (history_.empty() || (previousSelectedIndex_ < 0))
	{
		return false;
	}
	return true;
}

//==============================================================================
bool CommandManagerImpl::canRedo() const
{
	if (!history_.empty() && (previousSelectedIndex_ != (( int ) history_.size() - 1)))
	{
		return true;
	}
	return false;
}

//==============================================================================
void CommandManagerImpl::undo()
{
	flush();

	if (currentIndex_.value() < 0)
	{
		return;
	}
	currentIndex_.value( currentIndex_.value() - 1 );
}


//==============================================================================
void CommandManagerImpl::redo()
{
	flush();

	if (currentIndex_.value() >= ( int ) history_.size())
	{
		return;
	}
	currentIndex_.value( currentIndex_.value() + 1 );
}


//==============================================================================
VariantList & CommandManagerImpl::getHistory()
{
	flush();

	return history_;
}

//==============================================================================
IListModel & CommandManagerImpl::getMacros()
{
	return macros_;
}


ValueChangeNotifier< int > & CommandManagerImpl::getCurrentIndex()
{
	return currentIndex_;
}


//==============================================================================
void CommandManagerImpl::beginBatchCommand()
{
	notifyBeginMultiCommand();
	this->queueCommand( getClassIdentifier<BatchCommand>(), BatchCommandStage::Begin );
}

//==============================================================================
void CommandManagerImpl::endBatchCommand()
{
	this->queueCommand( getClassIdentifier<BatchCommand>(), BatchCommandStage::End );
}

//==============================================================================
void CommandManagerImpl::abortBatchCommand()
{
	this->queueCommand( getClassIdentifier<BatchCommand>(), BatchCommandStage::Abort );
}

//==============================================================================
void CommandManagerImpl::notifyBeginMultiCommand()
{
	multiCommandStatusChanged( ICommandEventListener::MultiCommandStatus_Begin );
}

//==============================================================================
void CommandManagerImpl::notifyCompleteMultiCommand()
{
	multiCommandStatusChanged( ICommandEventListener::MultiCommandStatus_Complete );
}

//==============================================================================
void CommandManagerImpl::notifyCancelMultiCommand()
{
	multiCommandStatusChanged( ICommandEventListener::MultiCommandStatus_Cancel );
}

//==============================================================================
void CommandManagerImpl::notifyHandleCommandQueued( const char * commandId )
{
	for ( auto & it : eventListenerCollection_ )
	{
		it->handleCommandQueued( commandId );
	}
}

//==============================================================================
void CommandManagerImpl::notifyNonBlockingProcessExecution( const char * commandId )
{
	for ( auto & it : eventListenerCollection_ )
	{
		it->onNonBlockingProcessExecution( commandId );
	}
}

//==============================================================================
void CommandManagerImpl::pushFrame( const CommandInstancePtr & instance )
{
	assert( !commandFrames_.empty() );
	assert( instance != nullptr );

	instance->setStatus( Running );

	if (commandFrames_.size() == 1 &&
		commandFrames_.front()->commandStack_.size() == 1)
	{
		// If the frame we are pushing is a root frame, we need to prep it
		// to monitor changes to the reflection data
		assert( instance != nullptr );
		instance->connectEvent();
	}
	else
	{
		auto currentFrame = commandFrames_.back();
		auto parentInstance = currentFrame->commandStack_.back();

		// TODO: support for custom undo
		/*if (instance->customUndo() || parentInstance->customUndo())
		{
			parentInstance->disconnectEvent();
			instance->connectEvent();
		}*/

		assert( instance->parent_ == nullptr );
		if (parentInstance != nullptr)
		{
			instance->parent_ = parentInstance;
			parentInstance->children_.push_back( instance );
		}
	}

	if (strcmp( instance->getCommandId(), typeid( BatchCommand ).name() ) == 0)
	{
		auto stage = instance->getArguments().getBase<BatchCommandStage>();
		assert( stage != nullptr );
		if (*stage == BatchCommandStage::Begin)
		{
			// The command stack represents BatchCommand groups within a frame.
			// On BeginBatchCommand we need to add a group to the stack
			commandFrames_.back()->commandStack_.push_back( instance );
		}
	}

	commandFrames_.push_back( new CommandFrame( instance ) );
}


//==============================================================================
namespace
{
	bool isBatchCommand(const ObjectHandleT<CommandInstance>& cmd)
	{
		return strcmp( cmd->getCommandId(), typeid( BatchCommand ).name() ) == 0;
	}
}


//==============================================================================
void CommandManagerImpl::popFrame()
{
	assert( !commandFrames_.empty() );
	auto currentFrame = commandFrames_.back();
	assert ( !currentFrame->commandStack_.empty() );
	auto instance = currentFrame->commandStack_.back();
	assert ( instance != nullptr );
	currentFrame->commandStack_.pop_back();
	assert( currentFrame->commandStack_.empty() && currentFrame->commandQueue_.empty() );
	delete commandFrames_.back();
	commandFrames_.pop_back();
	assert( !commandFrames_.empty() );
	currentFrame = commandFrames_.back();

	if (isBatchCommand( instance ))
	{
		auto stage = instance->getArguments().getBase<BatchCommandStage>();
		assert( stage != nullptr );
		// Set the arguments to nullptr since there is no need to serialize BatchCommand arguments
		instance->setArguments( nullptr );

		if (*stage != BatchCommandStage::Begin)
		{
			// If we are ending or aborting a BatchCommand, we need to remove its group from the stack
			assert ( !currentFrame->commandStack_.empty() );
			instance = currentFrame->commandStack_.back();
			assert ( instance != nullptr );
			currentFrame->commandStack_.pop_back();
			assert ( !currentFrame->commandStack_.empty() );
		}
	}

	if (commandFrames_.size() == 1 &&
		commandFrames_.front()->commandStack_.size() == 1)
	{
		// If we are popping a root command frame, finalise the undo/redo stream and add the command to history
		instance->disconnectEvent();

		if (instance->getErrorCode() == CommandErrorCode::NO_ERROR)
		{
			addToHistory( instance );
			if (instance->isMultiCommand())
			{
				notifyCompleteMultiCommand();
			}
		}
	}
	else
	{
		auto parentInstance = currentFrame->commandStack_.back();
		/*if (instance->customUndo() || parentInstance->customUndo())
		{
			instance->disconnectEvent();
			parentInstance->connectEvent();
		}*/
	}

	// TODO: This does not actually work for sub commands. No undo data is stored
	// for sub commands so calling undo does nothing.
	if (instance->getErrorCode() != CommandErrorCode::NO_ERROR)
	{
		instance->undo();
		if (instance->isMultiCommand())
		{
			notifyCancelMultiCommand();
		}
		if (!isBatchCommand( instance ) || instance->getErrorCode() != CommandErrorCode::ABORTED)
		{
			NGT_ERROR_MSG( "Failed to execute command %s \n", instance->getCommandId() );
		}
	}

	instance->setStatus( Complete );
}

//==============================================================================
void CommandManagerImpl::addToHistory( const CommandInstancePtr & instance )
{
	if (instance.get()->getCommand()->canUndo( instance.get()->getArguments() ))
	{
		pendingHistory_.push_back( instance );
	}
}

//==============================================================================
bool CommandManagerImpl::SaveCommandHistory( ISerializer & serializer )
{
	// save objects
	size_t count = history_.size();
	serializer.serialize( count );
	for(size_t i = 0; i < count; i++)
	{
		serializer.serialize( history_[i] );
	}
	// save history index
	serializer.serialize( currentIndex_.value() );
	return true;
}

//==============================================================================
bool CommandManagerImpl::LoadCommandHistory( ISerializer & serializer )
{
	// read history data
	size_t count = 0;
	serializer.deserialize( count );
	for(size_t i = 0; i < count; i++)
	{
		Variant variant;
		serializer.deserialize( variant );
		CommandInstancePtr ins;
		bool isOk = variant.tryCast( ins );
		assert( isOk );
		assert( ins != nullptr );
		ins->setCommandSystemProvider( pCommandManager_ );
		ins->setDefinitionManager(
			const_cast<IDefinitionManager&>(pCommandManager_->getDefManager()) );
		history_.emplace_back( std::move( variant ) );
	}
	int index = NO_SELECTION;
	serializer.deserialize( index );
	this->updateSelected( index );

	return true;
}

//==============================================================================
void CommandManagerImpl::multiCommandStatusChanged( ICommandEventListener::MultiCommandStatus status )
{
	for ( auto & it : eventListenerCollection_ )
	{
		it->multiCommandStatusChanged( status );
	}
}

//==============================================================================
void CommandManagerImpl::onPreDataChanged( const IValueChangeNotifier* sender,
										  const IValueChangeNotifier::PreDataChangedArgs& args )
{
	previousSelectedIndex_ = currentIndex_.value();
}

//==============================================================================
void CommandManagerImpl::onPostDataChanged( const IValueChangeNotifier* sender,
										   const IValueChangeNotifier::PostDataChangedArgs& args )
{
	static const char* id = typeid( UndoRedoCommand ).name();
	auto instance = queueCommand( id, currentIndex_.value() );
	waitForInstance( instance );
}

//==============================================================================
void CommandManagerImpl::onPostItemsRemoved( const IListModel* sender,
						const IListModel::PostItemsRemovedArgs& args )
{
	// update currentIndex when history_ items removed
	if(history_.empty())
	{
		updateSelected( NO_SELECTION );
	}
	else
	{
		const int & size = static_cast<int>(history_.size());
		if (currentIndex_.value() > size)
		{
			// goes here means history was corrupt.
			assert( false );
		}
	}
}


//==============================================================================
void CommandManagerImpl::processCommands()
{
	std::thread::id currentThreadId = std::this_thread::get_id();

	std::unique_lock<std::mutex> lock( workerMutex_ );

	for (;;)
	{
		auto commandFrame = commandFrames_.back();		
		if (commandFrame->commandQueue_.empty())
		{
			break;
		}

		auto job = commandFrame->commandQueue_.front();
		auto threadAffinity = job->getCommand()->threadAffinity();
		if (threadAffinity == CommandThreadAffinity::UI_THREAD &&
			currentThreadId != ownerThreadId_)
		{
			// The next command in the queue needs to be run on the UI thread.
			// Notify the owner thread
			ownerWakeUp_ = true;
			break;
		}
		else if (threadAffinity == CommandThreadAffinity::COMMAND_THREAD &&
			currentThreadId != workerThreadId_)
		{
			// The next command in the queue needs to be run on the command thread.
			// Notify the worker thread
			workerWakeUp_.notify_all();
			break;
		}

		commandFrame->commandQueue_.pop_front();

		if (strcmp( job->getCommandId(), typeid( UndoRedoCommand ).name() ) == 0)
		{
			//execute undo/redo
			lock.unlock(); // release lock while running commands
			job->setStatus( Running );
			job->execute();
			job->setStatus( Complete );
			lock.lock();
		}
		else
		{
			// Push a command frame for this job
			auto previousFrame = THREAD_LOCAL_GET( currentFrame_ );
			pushFrame( job );
			auto currentFrame = commandFrames_.back();
			THREAD_LOCAL_SET( currentFrame_, currentFrame );

			lock.unlock(); // release lock while running commands
			job->execute();
			lock.lock();

			// Spin and process commands until all sub commands for this frame have been executed
			while (!currentFrame->commandQueue_.empty() || commandFrames_.back() != currentFrame )
			{
				lock.unlock();
				processCommands();
				lock.lock();
			}

			// Pop the command frame
			THREAD_LOCAL_SET( currentFrame_, previousFrame );
			popFrame();
		}
	}

	if (currentThreadId != ownerThreadId_)
	{
		if (!pendingHistory_.empty())
		{
			ownerWakeUp_ = true;
		}
		return;
	}

	if (pendingHistory_.empty())
	{
		return;
	}

	if (static_cast<int>(history_.size()) > currentIndex_.value() + 1)
	{
		// erase all history after the current index as we have pending
		// history that will make this invalid
		history_.resize( currentIndex_.value() + 1 );
	}

	while (!pendingHistory_.empty())
	{
		history_.push_back( pendingHistory_.front() );
		pendingHistory_.pop_front();
	}
	updateSelected( static_cast< int >( history_.size() - 1 ) );
}


//==============================================================================
void CommandManagerImpl::flush()
{
	assert( std::this_thread::get_id() == ownerThreadId_ );

	std::unique_lock<std::mutex> lock( workerMutex_ );

	while (commandFrames_.size() > 1 ||
		!commandFrames_.front()->commandQueue_.empty() ||
		!pendingHistory_.empty())
	{
		lock.unlock();
		processCommands();
		lock.lock();
	}
}


//==============================================================================
/*static */void CommandManagerImpl::threadFunc()
{
	std::unique_lock<std::mutex> lock( workerMutex_ );

	while (exiting_ == false)
	{
		workerWakeUp_.wait( lock, [this]
		{
			auto & commandFrame = *commandFrames_.back();
			return
				!commandFrame.commandQueue_.empty() ||
				exiting_;
		});

		// execute commands
		lock.unlock();
		processCommands();
		lock.lock();
	}
}


}


//==============================================================================
CommandManager::CommandManager( const IDefinitionManager & defManager )
	: pImpl_( nullptr )
	, defManager_( defManager )
{
}


//==============================================================================
CommandManager::~CommandManager()
{
	if(pImpl_ != nullptr)
	{
		fini();
	}
}

//==============================================================================
void CommandManager::init( IApplication & application )
{
	if (pImpl_ == nullptr)
	{
		pImpl_ = new CommandManagerImpl( this );
	}
	pImpl_->init( application );
}


//==============================================================================
void CommandManager::fini()
{
	if (pImpl_ != nullptr)
	{
		pImpl_->fini();
	}
	delete pImpl_;
	pImpl_ = nullptr;
}


//==============================================================================
void CommandManager::registerCommand( Command * command )
{
	pImpl_->registerCommand( command );
}


//==============================================================================
void CommandManager::deregisterCommand( const char * commandId )
{
	pImpl_->deregisterCommand( commandId );
}

//==============================================================================
Command * CommandManager::findCommand(
	const char * commandId ) const
{
	return pImpl_->findCommand( commandId );
}


//==============================================================================
CommandInstancePtr CommandManager::queueCommand(
	const char * commandId, const ObjectHandle & arguments )
{
	return pImpl_->queueCommand( commandId, arguments );
}


//==============================================================================
void CommandManager::waitForInstance( const CommandInstancePtr & instance )
{
	pImpl_->waitForInstance( instance );
}


//==============================================================================
void CommandManager::registerCommandStatusListener(
	ICommandEventListener * listener )
{
	return pImpl_->registerCommandStatusListener( listener );
}


//==============================================================================
void CommandManager::fireCommandStatusChanged( const CommandInstance & command ) const
{
	return pImpl_->fireCommandStatusChanged( command );
}


//==============================================================================
void CommandManager::fireProgressMade( const CommandInstance & command ) const
{
	return pImpl_->fireProgressMade( command );
}


//==============================================================================
bool CommandManager::canUndo() const
{
	return pImpl_->canUndo();
}


//==============================================================================
bool CommandManager::canRedo() const
{
	return pImpl_->canRedo();
}


//==============================================================================
void CommandManager::undo()
{
	pImpl_->undo();
}


//==============================================================================
void CommandManager::redo()
{
	pImpl_->redo();
}


//==============================================================================
const VariantList & CommandManager::getHistory() const
{
	return pImpl_->getHistory();
}

//==============================================================================
const IListModel & CommandManager::getMacros() const
{
	return pImpl_->getMacros();
}


IValueChangeNotifier& CommandManager::currentIndex()
{
	return pImpl_->getCurrentIndex();
}


//==============================================================================
const IDefinitionManager & CommandManager::getDefManager() const
{
	return defManager_;
}


//==============================================================================
bool CommandManager::SaveHistory( ISerializer & serializer )
{
	return pImpl_->SaveCommandHistory( serializer );
}

//==============================================================================
bool CommandManager::LoadHistory( ISerializer & serializer )
{
	return pImpl_->LoadCommandHistory( serializer );
}


//==============================================================================
void CommandManager::beginBatchCommand()
{
	pImpl_->beginBatchCommand();
}

//==============================================================================
void CommandManager::endBatchCommand()
{
	pImpl_->endBatchCommand();
}

void CommandManager::abortBatchCommand()
{
	pImpl_->abortBatchCommand();
}

//==============================================================================
void CommandManager::notifyBeginMultiCommand()
{
	pImpl_->notifyBeginMultiCommand();
}

//==============================================================================
void CommandManager::notifyCompleteMultiCommand()
{
	pImpl_->notifyCompleteMultiCommand();
}

//==============================================================================
void CommandManager::notifyCancelMultiCommand()
{
	pImpl_->notifyCancelMultiCommand();
}

//==============================================================================
void CommandManager::notifyHandleCommandQueued( const char * commandId )
{
	pImpl_->notifyHandleCommandQueued( commandId );
}

//==============================================================================
void CommandManager::notifyNonBlockingProcessExecution( const char * commandId )
{
	pImpl_->notifyNonBlockingProcessExecution( commandId );
}

//==============================================================================
bool CommandManagerImpl::createCompoundCommand(
	const VariantList & commandInstanceList, const char * id )
{
	// create compound command
	std::vector< size_t > commandIndices;
	size_t count = commandInstanceList.size();
	{
		std::unique_lock<std::mutex> lock( workerMutex_ );
		for(size_t i = 0; i < count; i++)
		{
			const Variant & variant = commandInstanceList[i];
			auto && findIt =
				std::find( history_.begin(), history_.end(), variant );
			if (findIt == history_.end())
			{
				continue;
			}
			commandIndices.push_back( findIt - history_.begin() );
		}
	}

	if (commandIndices.empty())
	{
		NGT_ERROR_MSG( "Failed to create macros: no command history. \n" );
		return false;
	}
	auto macro = pCommandManager_->getDefManager().create<CompoundCommand>( false );
	macro->setId( id );
	pCommandManager_->registerCommand( macro.get() );
	std::sort( commandIndices.begin(), commandIndices.end() );
	auto indexIt =
		commandIndices.begin();
	auto indexItEnd =
		commandIndices.end();
	for( ; indexIt != indexItEnd; ++indexIt )
	{
		const CommandInstancePtr & instance = history_[*indexIt].value<CommandInstancePtr>();
		if (instance->isMultiCommand())
		{
			addBatchCommandToCompoundCommand( macro, instance );
		}
		else
		{
			macro->addCommand( instance->getCommandId(), instance->getArguments() );
		}
	}
	macro->initDisplayData( const_cast<IDefinitionManager&>(pCommandManager_->getDefManager()) );
	macros_.emplace_back( std::move( macro ) );
	return true;
}

//==============================================================================
void CommandManagerImpl::addBatchCommandToCompoundCommand(
	const ObjectHandleT<CompoundCommand>& compoundCommand,
	const CommandInstancePtr & instance )
{
	assert( !instance->children_.empty() );
	for (auto & child : instance->children_)
	{
		if (child->isMultiCommand())
		{
			addBatchCommandToCompoundCommand( compoundCommand, child );
		}
		else
		{
			if (strcmp(child->getCommandId(), typeid(BatchCommand).name()) != 0)
			{
				compoundCommand->addCommand( child->getCommandId(), child->getArguments() );
			}
		}
	}
}

//==============================================================================
bool CommandManager::createMacro( const VariantList & commandInstanceList, const char * id )
{
	static int index = 1;
	static const std::string defaultName("Macro");

	std::string macroName("");
	if (id != nullptr)
	{
		macroName = id;
	}
	if (findCommand( macroName.c_str() ) != nullptr )
	{
		NGT_ERROR_MSG( "Failed to create macros: macro name %s already exists. \n", macroName.c_str() );
		return false;
	}
	if (macroName.empty())
	{
		char buffer[260];
		do
		{
			snprintf( buffer, 260, "%d", index);
			macroName = defaultName + buffer;
			index++;
		}
		while (findCommand( macroName.c_str()) != nullptr);
	}
	return pImpl_->createCompoundCommand( commandInstanceList, macroName.c_str() );
}

//==============================================================================
bool CommandManagerImpl::deleteCompoundCommand( const char * id )
{
	bool bSuccess = false;
	CompoundCommand * compoundCommand = static_cast< CompoundCommand * >(findCommand( id ));
	if (compoundCommand != nullptr)
	{
		typedef GenericListT< ObjectHandleT< CompoundCommand > > MacroList;
		for(MacroList::Iterator iter = macros_.begin(); iter != macros_.end(); ++iter)
		{
			const  ObjectHandleT< CompoundCommand > & obj = *iter;
			bool isOk = ( strcmp( id, obj->getId() ) == 0);
			if(isOk)
			{
				deregisterCommand( id );
				macros_.erase( iter );
				bSuccess = true;
				break;
			}
		}
	}
	return bSuccess;
}

//==============================================================================
bool CommandManager::deleteMacroByName( const char * id )
{
	return pImpl_->deleteCompoundCommand( id );
}

//==============================================================================
void CommandManager::addToHistory( const CommandInstancePtr & instance )
{
	pImpl_->addToHistory( instance );
}

//==============================================================================
bool CommandManager::undoRedo( const int & desiredIndex )
{
	assert( pImpl_ != nullptr );
	auto & history = this->getHistory();
	const int & size = static_cast<int>(history.size());
	if (size == 0)
	{
		assert( false );
		return false;
	}
	if ((pImpl_->previousSelectedIndex_ == desiredIndex) || (desiredIndex >= size))
	{
		assert( false );
		return false;
	}
	while (pImpl_->previousSelectedIndex_ != pImpl_->currentIndex_.value())
	{
		if (pImpl_->previousSelectedIndex_ > pImpl_->currentIndex_.value())
		{
			int i = pImpl_->previousSelectedIndex_;
			CommandInstancePtr job = history[i].value<CommandInstancePtr>();
			job->undo();
			pImpl_->previousSelectedIndex_--;
		}
		else
		{
			int i = pImpl_->previousSelectedIndex_;
			CommandInstancePtr job = history[i+1].value<CommandInstancePtr>();
			job->redo();
			pImpl_->previousSelectedIndex_++;
		}
	}
	return true;
}
