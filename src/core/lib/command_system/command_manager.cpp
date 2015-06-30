#include "command_manager.hpp"
#include "command_instance.hpp"
#include "compound_command.hpp"
#include "i_command_event_listener.hpp"

#include "data_model/generic_list.hpp"
#include "data_model/value_change_notifier.hpp"
#include "variant/variant.hpp"
#include "wg_types/hashed_string_ref.hpp"
#include "reflection/property_accessor.hpp"
#include "reflection/utilities/reflection_utilities.hpp"
#include "reflection/i_definition_manager.hpp"
#include "serialization/serializer/i_serialization_manager.hpp"
#include "logging/logging.hpp"
#include "batch_command.hpp"
#include <deque>
#include <thread>
#include <map>
#include <mutex>
#include "ngt_core_common/wg_condition_variable.hpp"


namespace
{

class CommandManagerImpl
{
public:
	static const int NO_SELECTION = -1;

	CommandManagerImpl( CommandManager* pCommandManager )
		: lastErrorCode_( NGT_NO_ERROR )
		, workerMutex_()
		, workerWakeUp_()
		, undoRedoSetDone_()
		, commands_()
		, commandQueue_()
		, history_()
		, macros_()
		, eventListenerCollection_()
		, globalEventListener_()
		, exiting_( false )
		, inited_( false )
		, currentIndex_( NO_SELECTION )
		, previousSelectedIndex_( NO_SELECTION )
		, workerThreadId_()
		, pCommandManager_( pCommandManager )
		, workerThread_()
		, activeInstances_()
		, beginBatchCommand_( pCommandManager )
		, endBatchCommand_( pCommandManager )
		, abortBatchCommand_( pCommandManager )
	{
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

		workerThread_.join();

		currentIndex_.onPreDataChanged().remove< CommandManagerImpl,
			&CommandManagerImpl::onPreDataChanged >( this );
		currentIndex_.onPostDataChanged().remove< CommandManagerImpl,
			&CommandManagerImpl::onPostDataChanged >( this );

		pCommandManager_ = nullptr;
		}

	void init();
	void registerCommand( Command * command );
	void deregisterCommand( const char * commandName );
	Command * findCommand(const char * commandName ) const;
	CommandInstancePtr queueCommand(
		const char * commandName, const ObjectHandle & arguments );
	CommandInstancePtr executeCommand(
		const char * commandName, const ObjectHandle & arguments );
	void registerCommandStatusListener( ICommandEventListener * listener );
	void fireCommandStatusChanged( const CommandInstance & command ) const;
	void fireProgressMade( const CommandInstance & command ) const;
	void setSelected( const int & value );
	const int& getSelected() const;
	void updateSelected( const int & value );

	void undo();
	void redo();
	bool canUndo() const;
	bool canRedo() const;
	GenericList & getHistory();
	ValueChangeNotifier< int > & getCurrentIndex();
	GenericList & getMacros();

	void beginBatchCommand();
	void endBatchCommand();
	void abortBatchCommand();
	void notifyBeginMultiCommand();
	void notifyCompleteMultiCommand();
	void notifyCancelMultiCommand();
	void notifyHandleCommandQueued( const char * commandId );
	void notifyNonBlockingProcessExecution( const char * commandId );
	CommandInstancePtr getActiveInstance() const;
	void pushActiveInstance( const CommandInstancePtr & instance );
	CommandInstancePtr popActiveInstance();
	void createCompoundCommand( const GenericList & commandInstanceList, const char * id );
	void deleteCompoundCommand( const char * id );
	void addToHistory( const CommandInstancePtr & instance );
	void threadFunc();

	NGTCommandErrorCode						lastErrorCode_;

private:
	typedef std::unordered_map< HashedStringRef, Command * > CommandCollection;
	typedef std::deque< CommandInstancePtr > CommandQueueCollection;
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
	- previousSelectedIndex_ != currentIndex_
	- exiting_ == true
	*/
	wg_condition_variable					workerWakeUp_;

	wg_condition_variable					undoRedoSetDone_; // assumed predicate: previousSelectedIndex_ == currentIndex_

	CommandCollection						commands_;
	CommandQueueCollection					commandQueue_;
	GenericList								history_;
	GenericList								macros_;
	EventListenerCollection					eventListenerCollection_;
	std::unique_ptr< ICommandEventListener > globalEventListener_;

	bool									exiting_;
	bool									inited_;
	ValueChangeNotifier< int >				currentIndex_;
	int										previousSelectedIndex_;
	std::thread::id							workerThreadId_;
	CommandManager*							pCommandManager_;
	std::thread								workerThread_;
	std::vector< CommandInstancePtr >		activeInstances_;
	BeginBatchCommand						beginBatchCommand_;
	EndBatchCommand							endBatchCommand_;
	AbortBatchCommand						abortBatchCommand_;
	
	void setSelectedIndexInternal( std::unique_lock<std::mutex>& lock );
	void multiCommandStatusChanged( ICommandEventListener::MultiCommandStatus status );
	void onPreDataChanged( const IValueChangeNotifier* sender,
		const IValueChangeNotifier::PreDataChangedArgs& args );
	void onPostDataChanged( const IValueChangeNotifier* sender,
		const IValueChangeNotifier::PostDataChangedArgs& args );
};

//==============================================================================
void CommandManagerImpl::init()
{
	if (inited_)
	{
		return;
	}
	inited_ = true;
	CommandManagerEventListener *
		listener = new CommandManagerEventListener();
	listener->setCommandSystemProvider( pCommandManager_ );
	globalEventListener_.reset( listener );

	registerCommand( &beginBatchCommand_ );
	registerCommand( &endBatchCommand_ );
	registerCommand( &abortBatchCommand_ );

	currentIndex_.onPreDataChanged().add< CommandManagerImpl,
		&CommandManagerImpl::onPreDataChanged >( this );
	currentIndex_.onPostDataChanged().add< CommandManagerImpl,
		&CommandManagerImpl::onPostDataChanged >( this );

	workerThread_ = std::thread( &CommandManagerImpl::threadFunc, this );
	workerThreadId_ = workerThread_.get_id();
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
		return nullptr;
	}
	auto instance =
		pCommandManager_->getDefManager().createT< CommandInstance >();
	instance->setCommandSystemProvider( pCommandManager_ );
	instance->setCommandId( command ->getId() );
	instance->setArguments( arguments );
	instance->init( workerThreadId_ );
	instance->setStatus( Queued );

	std::unique_lock<std::mutex> lock( workerMutex_ );
	commandQueue_.push_back( instance );
	workerWakeUp_.notify_all();

	return instance;
}

//==============================================================================
CommandInstancePtr CommandManagerImpl::executeCommand(
	const char * commandId, const ObjectHandle & arguments )
{
	Command * command = findCommand( commandId );
	if(command == nullptr)
	{
		return nullptr;
	}
	auto instance =
		pCommandManager_->getDefManager().createT< CommandInstance >();
	instance->setCommandSystemProvider( pCommandManager_ );
	instance->setCommandId( command ->getId() );
	instance->setArguments( arguments );
	instance->init( workerThreadId_ );
	instance->setStatus( Queued );

	if (getActiveInstance() != nullptr)
	{
		pushActiveInstance( instance );
		instance->execute();
		popActiveInstance();
	}
	else
	{
		workerMutex_.lock();
		commandQueue_.push_back( instance );
		workerWakeUp_.notify_all();
		workerMutex_.unlock();

		instance->waitForCompletion();
	}

	return instance;
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
void CommandManagerImpl::setSelected( const int & value )
{
	std::unique_lock<std::mutex> lock( workerMutex_ );
	if (previousSelectedIndex_ == value)
	{
		return;
	}
	int size = static_cast<int>(history_.size());
	if (size == 0)
	{
		assert( value == NO_SELECTION );
		return;
	}
	if (value >= size)
	{
		return;
	}

	if (std::this_thread::get_id() == workerThreadId_)
	{
		setSelectedIndexInternal( lock );
		return;
	}

	workerWakeUp_.notify_all();

}


//==============================================================================
const int& CommandManagerImpl::getSelected() const
{
	return currentIndex_.value();
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
	if (previousSelectedIndex_ < 0)
	{
		return false;
	}
	return true;
}

//==============================================================================
bool CommandManagerImpl::canRedo() const
{
	if (previousSelectedIndex_ != (( int ) history_.size() - 1))
	{
		return true;
	}
	return false;
}

//==============================================================================
void CommandManagerImpl::undo()
{
	if (currentIndex_.value() < 0)
	{
		return;
	}
	currentIndex_.value( currentIndex_.value() - 1 );
}


//==============================================================================
void CommandManagerImpl::redo()
{
	if (currentIndex_.value() >= ( int ) history_.size())
	{
		return;
	}
	currentIndex_.value( currentIndex_.value() + 1 );
}


//==============================================================================
GenericList & CommandManagerImpl::getHistory()
{
	return history_;
}

//==============================================================================
GenericList & CommandManagerImpl::getMacros()
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

	auto instance =
		pCommandManager_->getDefManager().createT< CommandInstance >();
	instance->setCommandSystemProvider( pCommandManager_ );
	instance->setCommandId( getClassIdentifier<BeginBatchCommand>() );
	instance->setArguments( nullptr );
	instance->init( workerThreadId_ );
	instance->setStatus( Queued );

	std::unique_lock<std::mutex> lock( workerMutex_ );
	commandQueue_.push_back( instance );
	workerWakeUp_.notify_all();
}

//==============================================================================
void CommandManagerImpl::endBatchCommand()
{
	auto instance =
		pCommandManager_->getDefManager().createT< CommandInstance >();
	instance->setCommandSystemProvider( pCommandManager_ );
	instance->setCommandId( getClassIdentifier<EndBatchCommand>() );
	instance->setArguments( nullptr );
	instance->init( workerThreadId_ );
	instance->setStatus( Queued );

	std::unique_lock<std::mutex> lock( workerMutex_ );
	commandQueue_.push_back( instance );
	workerWakeUp_.notify_all();
}

//==============================================================================
void CommandManagerImpl::abortBatchCommand()
{
	auto instance =
		pCommandManager_->getDefManager().createT< CommandInstance >();
	instance->setCommandSystemProvider( pCommandManager_ );
	instance->setCommandId( getClassIdentifier<AbortBatchCommand>() );
	instance->setArguments( nullptr );
	instance->init( workerThreadId_ );
	instance->setStatus( Queued );

	std::unique_lock<std::mutex> lock( workerMutex_ );
	commandQueue_.push_back( instance );
	workerWakeUp_.notify_all();
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
CommandInstancePtr CommandManagerImpl::getActiveInstance() const
{
	if (activeInstances_.empty())
	{
		return nullptr;
	}

	return activeInstances_.back();
}

//==============================================================================
void CommandManagerImpl::pushActiveInstance( const CommandInstancePtr & instance )
{
	if (activeInstances_.empty())
	{
		instance->connectEvent();
	}
	else if (instance != nullptr)
	{
		auto parentInstance = activeInstances_.back();
		/*if (instance->customUndo() || parentInstance->customUndo())
		{
			parentInstance->disconnectEvent();
			instance->connectEvent();
		}*/
		parentInstance->addChild( instance );
	}
	activeInstances_.push_back( instance );
}

//==============================================================================
CommandInstancePtr CommandManagerImpl::popActiveInstance()
{
	assert( !activeInstances_.empty() );
	auto instance = activeInstances_.back();
	activeInstances_.pop_back();
	if (activeInstances_.empty())
	{
		instance->disconnectEvent();
	}
	else if (instance != nullptr)
	{
		auto parentInstance = activeInstances_.back();
		/*if (instance->customUndo() || parentInstance->customUndo())
		{
			instance->disconnectEvent();
			parentInstance->connectEvent();
		}*/
	}
	return instance;
}


//==============================================================================
void CommandManagerImpl::addToHistory( const CommandInstancePtr & instance )
{
	history_.emplace_back( Variant( instance ) );
	updateSelected( static_cast< int >( history_.size() - 1 ) );
}

//==============================================================================
void CommandManagerImpl::setSelectedIndexInternal( std::unique_lock<std::mutex>& lock )
{
	// Release lock while running undo/redo.
	// desiredSelectedIndex_ may change as side effect of undo/redo, so recheck
	// moving direction each iteration
	while (previousSelectedIndex_ != currentIndex_.value())
	{
		lastErrorCode_ = NGT_NO_ERROR;
		if (previousSelectedIndex_ > currentIndex_.value())
		{
			int i = previousSelectedIndex_;
			CommandInstancePtr job = history_[i].value<CommandInstancePtr>();

			lock.unlock();
			job->undo();
			lock.lock();

			previousSelectedIndex_--;
		}
		else
		{
			int i = previousSelectedIndex_;
			CommandInstancePtr job = history_[i+1].value<CommandInstancePtr>();

			lock.unlock();
			job->redo();
			lock.lock();
			previousSelectedIndex_++;
		}
	}

	undoRedoSetDone_.notify_all();
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
	this->setSelected( currentIndex_.value() );
}


//==============================================================================
/*static */void CommandManagerImpl::threadFunc()
{
	std::unique_lock<std::mutex> lock( workerMutex_ );

	while (exiting_ == false)
	{
		workerWakeUp_.wait( lock, [this]
		{
			return 
				!commandQueue_.empty() ||
				previousSelectedIndex_ != currentIndex_.value() ||
				exiting_;
		});

		// execute commands
		if (!commandQueue_.empty())
		{
			CommandQueueCollection collection;
			commandQueue_.swap(collection);

			lock.unlock(); // release lock while running commands

			while (collection.empty() == false)
			{
				CommandInstancePtr job = collection.front();
				collection.pop_front();

				if (getActiveInstance() == nullptr)
				{
					if (static_cast<int>(history_.size()) > currentIndex_.value() + 1)
					{
						history_.resize( currentIndex_.value() + 1 );
					}
				}

				pushActiveInstance( job );
				job->execute();
				auto instance = popActiveInstance();

				// instance may not be the same commandInstance as job.
				// for example an endBatchCommand job will return a 
				// beginBatchCommand instance
				if (instance == nullptr)
				{
					continue;
				}

				if (lastErrorCode_ != NGT_NO_ERROR)
				{
					instance->undo();
					if (instance->isMultiCommand())
					{
						notifyCancelMultiCommand();
					}
					NGT_ERROR_MSG( "Failed to execute command %s \n", instance->getCommandId() );
				}
				else if (getActiveInstance() == nullptr)
				{
					addToHistory( instance );
					if (instance->isMultiCommand())
					{
						notifyCompleteMultiCommand();
					}
				}
			}

			lock.lock();
		}

		// set history position
		setSelectedIndexInternal( lock );
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
void CommandManager::init()
{
	if (pImpl_ == nullptr)
	{ 
		pImpl_ = new CommandManagerImpl( this );
	}
	pImpl_->init();
}


//==============================================================================
void CommandManager::fini()
{
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
CommandInstancePtr CommandManager::executeCommand(
	const char * commandId, const ObjectHandle & arguments )
{
	return pImpl_->executeCommand( commandId, arguments );
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
const GenericList & CommandManager::getHistory() const
{
	return pImpl_->getHistory();
}

//==============================================================================
const GenericList & CommandManager::getMacros() const
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
bool CommandManager::SaveHistory( ISerializationManager & serializationMgr, IDataStream & stream )
{
	// save objects
	const GenericList & history = pImpl_->getHistory();
	size_t count = history.size();
	stream.write( count );
	for(size_t i = 0; i < count; i++)
	{
		const Variant & variant = history[i].value<const Variant &>();
		stream.write( variant.type()->name());
		serializationMgr.serialize( stream, variant );
	}
	// save history index
	const int index = pImpl_->getSelected();
	stream.write( index );
	return true;
}

//==============================================================================
bool CommandManager::LoadHistory( ISerializationManager & serializationMgr, IDataStream & stream )
{
	// read history data
	size_t count = 0;
	stream.read( count );
	GenericList & history = pImpl_->getHistory();
	for(size_t i = 0; i < count; i++)
	{
		std::string valueType;
		stream.read( valueType );
		const MetaType* metaType = Variant::getMetaTypeManager()->findType( valueType.c_str() );
		assert( metaType != nullptr );
		Variant variant( metaType );
		serializationMgr.deserialize( stream, variant );
		CommandInstancePtr ins;
		bool isOk = variant.tryCast( ins );
		assert( isOk );
		assert( ins != nullptr );
		ins->setCommandSystemProvider( this );
		history.emplace_back( std::move( variant ) );
	}
	int index = CommandManagerImpl::NO_SELECTION;
	stream.read( index );
	pImpl_->updateSelected( index );

	return true;
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
CommandInstancePtr CommandManager::getActiveInstance() const
{
	return pImpl_->getActiveInstance();
}

//==============================================================================
void CommandManager::pushActiveInstance( const CommandInstancePtr & instance )
{
	pImpl_->pushActiveInstance( instance );
}

//==============================================================================
CommandInstancePtr CommandManager::popActiveInstance()
{
	return pImpl_->popActiveInstance();
}

//==============================================================================
void CommandManagerImpl::createCompoundCommand(
	const GenericList & commandInstanceList, const char * id )
{
	// create compound command
	std::vector< size_t > commandIndices;
	size_t count = commandInstanceList.size();
	{
		std::unique_lock<std::mutex> lock( workerMutex_ );
		for(size_t i = 0; i < count; i++)
		{
			const Variant & variant = commandInstanceList[i].value<const Variant &>();
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
		return;
	}
	auto macro = pCommandManager_->getDefManager().createT<CompoundCommand>( false );
	macro->setId( id );
	pCommandManager_->registerCommand( macro.get() );
	std::sort( commandIndices.begin(), commandIndices.end() );
	auto indexIt =
		commandIndices.begin();
	auto indexItEnd =
		commandIndices.end();
	for( ; indexIt != indexItEnd; ++indexIt )
	{
		macro->addCommand( pCommandManager_->getDefManager(),
			history_[*indexIt].value<CommandInstancePtr>() );
	}
	macro->initDisplayData( const_cast<IDefinitionManager&>(pCommandManager_->getDefManager()) );
	macros_.emplace_back( macro );
}

//==============================================================================
void CommandManager::createCompoundCommand( const GenericList & commandInstanceList, const char * id )
{
	static int index = 1;
	static const std::string defaultName("Macro");

	std::string macroName("");
	if (id != nullptr)
	{
		macroName = id;
	}
	if(findCommand( macroName.c_str() ) != nullptr )
	{
		NGT_ERROR_MSG( "Failed to create macros: macro name %s already exists. \n", macroName.c_str() );
		return;
	}
	if(macroName.empty())
	{
		do
		{
			char buffer[260];
			_snprintf( buffer, 260, "%d", index);
			macroName = defaultName + buffer;
			index++;
		}
		while(findCommand( macroName.c_str()) != nullptr);
	}
	pImpl_->createCompoundCommand( commandInstanceList, macroName.c_str() );
}

//==============================================================================
void CommandManagerImpl::deleteCompoundCommand( const char * id )
{
	CompoundCommand * compoundCommand = static_cast< CompoundCommand * >(findCommand( id ));
	if (compoundCommand != nullptr)
	{
		for(GenericList::Iterator iter = macros_.begin(); iter != macros_.end(); ++iter)
		{
			const Variant & variant = (*iter).value<const Variant &>();
			ObjectHandleT<CompoundCommand> obj;
			bool isOk = variant.tryCast( obj );
			if (isOk)
			{
				isOk = ( strcmp( id, obj->getId() ) == 0);
			}
			if(isOk)
			{
				deregisterCommand( id );
				macros_.erase( iter );
				break;
			}
		}
	}
}

//==============================================================================
void CommandManager::deleteCompoundCommand( const char * id )
{
	pImpl_->deleteCompoundCommand( id );
}

//==============================================================================
void CommandManager::addToHistory( const CommandInstancePtr & instance )
{
	pImpl_->addToHistory( instance );
}


//==============================================================================
NGTCommandErrorCode CommandManager::getLastError() const
{
	return pImpl_->lastErrorCode_;
}

//==============================================================================
void CommandManager::setErrorCode( NGTCommandErrorCode errorCode )
{
	pImpl_->lastErrorCode_ = errorCode;
}
