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

#include <atomic>
#include <deque>
#include <thread>
#include <map>


//TODO: Switch to multiplatform wait handles, possibly C++11
#if defined( _WIN32 )
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif // defined( _WIN32 )


namespace
{

class BeginBatchCommand
	: public Command
{

public:
	BeginBatchCommand( CommandManager * pCommandManager )
		: pCommandManager_( pCommandManager )
	{
	}
	const char * getId() const override
	{
		static const char * s_id = getClassIdentifier<BeginBatchCommand>();
		return s_id;
	}
	ObjectHandle execute(
		const ObjectHandle & arguments ) const override
	{
		assert( pCommandManager_ != nullptr );
		assert( pCommandManager_->getActiveInstance() == nullptr );
		pCommandManager_->notifyBeginMultiCommand();
		pCommandManager_->pushActiveInstance();
		return nullptr;
	}

	void undo( IDataStream & dataStore ) const override{}
	void redo( IDataStream & dataStore ) const override{}

private:
	CommandManager * pCommandManager_;
};

class EndBatchCommand
	: public Command
{

public:
	EndBatchCommand( CommandManager * pCommandManager )
		: pCommandManager_( pCommandManager )
	{
	}
	const char * getId() const override
	{
		static const char * s_id = getClassIdentifier<EndBatchCommand>();
		return s_id;
	}
	ObjectHandle execute(
		const ObjectHandle & arguments ) const override
	{
		assert( pCommandManager_ != nullptr );
		auto activeInstance = pCommandManager_->getActiveInstance();
		assert( activeInstance != nullptr );
		pCommandManager_->popActiveInstance();
		pCommandManager_->addToHistory( activeInstance );
		pCommandManager_->notifyCompleteMultiCommand();
		return nullptr;
	}

	void undo( IDataStream & dataStore ) const override{}
	void redo( IDataStream & dataStore ) const override{}

private:
	CommandManager * pCommandManager_;
};

class AbortBatchCommand
	: public Command
{

public:
	AbortBatchCommand( CommandManager * pCommandManager )
		: pCommandManager_( pCommandManager )
	{
	}
	const char * getId() const override
	{
		static const char * s_id = getClassIdentifier<AbortBatchCommand>();
		return s_id;
	}
	ObjectHandle execute(
		const ObjectHandle & arguments ) const override
	{
		assert( pCommandManager_ != nullptr );
		auto activeInstance = pCommandManager_->getActiveInstance();
		assert( activeInstance != nullptr );
		pCommandManager_->popActiveInstance();
		activeInstance->undo();
		pCommandManager_->notifyCancelMultiCommand();
		return nullptr;
	}

	void undo( IDataStream & dataStore ) const override{}
	void redo( IDataStream & dataStore ) const override{}

private:
	CommandManager * pCommandManager_;
};

class CommandManagerImpl
{
public:
	static const int NO_SELECTION = -1;

	CommandManagerImpl( CommandManager* pCommandManager )
		: exiting_( false )
		, commandQueue_( new CommandQueueCollection() )
		, inited_( false )
		, currentIndex_( NO_SELECTION )
		, pCommandManager_( pCommandManager )
		, beginBatchCommand_( pCommandManager )
		, endBatchCommand_( pCommandManager )
		, abortBatchCommand_( pCommandManager )
	{
	}

	~CommandManagerImpl()
	{
		pCommandManager_ = nullptr;
		exiting_ = true;
		SetEvent( handles_[EXIT_COMMAND] );
		pThread_->join();
		commands_.clear();
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
	CommandInstancePtr getActiveInstance();
	void pushActiveInstance();
	void popActiveInstance();
	void createCompoundCommand( GenericList & commandInstanceList, const char * id );
	void deleteCompoundCommand( const char * id );
	void addToHistory( const CommandInstancePtr & instance );
	void threadFunc();

private:
	enum EventHandle
	{
		COMMAND_QUEUE = 0,
		UNDO_REDO_SET_DIRECT,
		CREATE_COMPOUND_COMMAND,
		EXIT_COMMAND,
		EVENT_HANDLES
	};

	enum DoneEventHandle
	{
		UNDO_REDO_SET_DONE = 0,
		CREATE_COMPOUND_COMMAND_DONE,
		DONE_EVENT_HANDLES
	};

	typedef std::unordered_map< HashedStringRef, Command * > CommandCollection;
	typedef std::deque< CommandInstancePtr > CommandQueueCollection;
	typedef std::list< ICommandEventListener * > EventListenerCollection;
	CommandCollection						commands_;
	std::atomic< CommandQueueCollection * >	commandQueue_;
	GenericList								history_;
	GenericList								macros_;
	EventListenerCollection					eventListenerCollection_;
	std::unique_ptr< ICommandEventListener > globalEventListener_;

	std::array< HANDLE, EVENT_HANDLES >		handles_;
	std::array< HANDLE, DONE_EVENT_HANDLES > doneHandles_;
	bool									exiting_;
	bool									inited_;
	ValueChangeNotifier< int >				currentIndex_;
	int										desiredSelectedIndex_;
	std::thread::id							threadId_;
	CommandManager*							pCommandManager_;
	GenericList *							pCommandInsList_;
	std::unique_ptr< std::thread >			pThread_;
	std::vector< CommandInstancePtr >		activeInstances_;
	BeginBatchCommand						beginBatchCommand_;
	EndBatchCommand							endBatchCommand_;
	AbortBatchCommand						abortBatchCommand_;
	std::mutex								macroCreationLock_;
	std::string								macroName_;
	ObjectHandleT<CompoundCommand>			currentMacro_;
	void setSelectedIndexInternal();
	void multiCommandStatusChanged( ICommandEventListener::MultiCommandStatus status );
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
	pThread_.reset(
		new std::thread( &CommandManagerImpl::threadFunc, this ) );
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
		pCommandManager_->getDefManager().createT< CommandInstance >( false );
	instance->setCommandSystemProvider( pCommandManager_ );
	instance->setCommandId( command ->getId() );
	instance->setArguments( arguments );
	instance->init( threadId_ );
	instance->setStatus( Queued );
	commandQueue_.load()->push_back( instance );
	SetEvent( handles_[ COMMAND_QUEUE ] );
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
		pCommandManager_->getDefManager().createT< CommandInstance >( false );
	instance->setCommandSystemProvider( pCommandManager_ );
	instance->setCommandId( command ->getId() );
	instance->setArguments( arguments );
	instance->init( threadId_ );
	instance->setStatus( Queued );
	if (getActiveInstance() != nullptr)
	{
		pushActiveInstance();
		instance->execute();
		popActiveInstance();
	}
	else
	{
		commandQueue_.load()->push_back( instance );
		SetEvent( handles_[ COMMAND_QUEUE ] );
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
	if (value >= ( int ) ( history_.size() ) ||
		desiredSelectedIndex_ == value )
	{
		return;
	}
	desiredSelectedIndex_ = value;
	if (std::this_thread::get_id() == threadId_)
	{
		setSelectedIndexInternal();
		return;
	}
	SetEvent( handles_[ UNDO_REDO_SET_DIRECT ] );
	const auto index = value < 0 ? 0 : value;
	CommandInstancePtr command =
		history_[index].value<CommandInstancePtr>();
	/*while( WaitForSingleObject(
		doneHandles_[ UNDO_REDO_SET_DONE ], 100 ) == WAIT_TIMEOUT )
	{
		EventListenerCollection::const_iterator it =
			eventListenerCollection_.begin();
		EventListenerCollection::const_iterator itEnd =
			eventListenerCollection_.end();
		for( ; it != itEnd; ++it )
		{ 
			(*it)->progressMade( *command.get() );
		}
	}*/
}


//==============================================================================
const int& CommandManagerImpl::getSelected() const
{
	return currentIndex_.value();
}


//==============================================================================
void CommandManagerImpl::updateSelected( const int & value )
{
	currentIndex_.value( value );
}


//==============================================================================
bool CommandManagerImpl::canUndo() const
{
	if ((history_.size() == 0) || (currentIndex_.value() < 0))
	{
		return false;
	}
	return true;
}

//==============================================================================
bool CommandManagerImpl::canRedo() const
{
	if ((history_.size() != 0) &&
		(currentIndex_.value() != (( int ) history_.size() - 1)))
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
	this->setSelected( currentIndex_.value() - 1 );
}


//==============================================================================
void CommandManagerImpl::redo()
{
	if (currentIndex_.value() >= ( int ) history_.size())
	{
		return;
	}
	this->setSelected( currentIndex_.value() + 1 );
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
		pCommandManager_->getDefManager().createT< CommandInstance >( false );
	instance->setCommandSystemProvider( pCommandManager_ );
	instance->setCommandId( getClassIdentifier<BeginBatchCommand>() );
	instance->setArguments( nullptr );
	instance->init( threadId_ );
	instance->setStatus( Queued );
	commandQueue_.load()->push_back( instance );
	SetEvent( handles_[ COMMAND_QUEUE ] );
}

//==============================================================================
void CommandManagerImpl::endBatchCommand()
{
	auto instance =
		pCommandManager_->getDefManager().createT< CommandInstance >( false );
	instance->setCommandSystemProvider( pCommandManager_ );
	instance->setCommandId( getClassIdentifier<EndBatchCommand>() );
	instance->setArguments( nullptr );
	instance->init( threadId_ );
	instance->setStatus( Queued );
	commandQueue_.load()->push_back( instance );
	SetEvent( handles_[ COMMAND_QUEUE ] );
}

//==============================================================================
void CommandManagerImpl::abortBatchCommand()
{
	auto instance =
		pCommandManager_->getDefManager().createT< CommandInstance >( false );
	instance->setCommandSystemProvider( pCommandManager_ );
	instance->setCommandId( getClassIdentifier<AbortBatchCommand>() );
	instance->setArguments( nullptr );
	instance->init( threadId_ );
	instance->setStatus( Queued );
	commandQueue_.load()->push_back( instance );
	SetEvent( handles_[ COMMAND_QUEUE ] );
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
CommandInstancePtr CommandManagerImpl::getActiveInstance()
{
	if (activeInstances_.empty())
	{
		return nullptr;
	}

	return activeInstances_.back();
}

//==============================================================================
void CommandManagerImpl::pushActiveInstance()
{
	auto activeInstance = pCommandManager_->getDefManager().createT< CommandInstance >( false );
	activeInstance->setCommandSystemProvider( pCommandManager_ );
	activeInstance->setCommandId( getClassIdentifier<BeginBatchCommand>() );
	activeInstance->setArguments( nullptr );
	activeInstance->init( threadId_ );
	if (activeInstances_.empty())
	{
		activeInstance->connectEvent();
	}
	activeInstances_.push_back( activeInstance );
}

//==============================================================================
void CommandManagerImpl::popActiveInstance()
{
	assert( !activeInstances_.empty() );
	if (activeInstances_.size() == 1)
	{
		activeInstances_.back()->disconnectEvent();
	}
	activeInstances_.pop_back();
}


//==============================================================================
void CommandManagerImpl::addToHistory( const CommandInstancePtr & instance )
{
	history_.emplace_back( Variant( instance ) );
	currentIndex_.value(
		static_cast< int >( history_.size() - 1 ) );
	desiredSelectedIndex_ = currentIndex_.value();
}

//==============================================================================
void CommandManagerImpl::setSelectedIndexInternal()
{
	if (desiredSelectedIndex_ == currentIndex_.value())
	{
		return;
	}
	if (desiredSelectedIndex_ < currentIndex_.value())
	{
		for(int i = currentIndex_.value(); i > desiredSelectedIndex_; --i )
		{
			CommandInstancePtr job = history_[i].value<CommandInstancePtr>();
			job->undo();
		}
	}
	else
	{
		for(int i = currentIndex_.value() + 1; i <= desiredSelectedIndex_; ++i)
		{
			CommandInstancePtr job = history_[i].value<CommandInstancePtr>();
			job->redo();
		}
	}
	currentIndex_.value( desiredSelectedIndex_ );
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
/*static */void CommandManagerImpl::threadFunc()
{
	for( size_t i = 0; i < handles_.size(); ++i )
	{
		handles_[ i ] = CreateEvent( 0, false, false, NULL );
	}

	for( size_t i = 0; i < doneHandles_.size(); ++i )
	{
		doneHandles_[ i ] = CreateEvent( 0, false, false, NULL );
	}

	threadId_ = std::this_thread::get_id();

	typedef std::unique_ptr< CommandQueueCollection > CommandQueueCollectionPtr;
	while (exiting_ == false)
	{
		DWORD waitVal = ::WaitForMultipleObjects(
			EVENT_HANDLES, &handles_[ 0 ], false, 1000 );
		switch ( waitVal - WAIT_OBJECT_0 )
		{
		case COMMAND_QUEUE:
			{
				CommandQueueCollection * newCollection =
					new CommandQueueCollection();
				CommandQueueCollectionPtr collection(
					commandQueue_.exchange( newCollection ) );
				while (collection->empty() == false)
				{
					CommandInstancePtr job = collection->front();
					auto activeInstance = getActiveInstance();
					if (activeInstance == nullptr )
					{
						if(strcmp(job->getCommandId(), getClassIdentifier<BeginBatchCommand>()) != 0)
						{
							pushActiveInstance();
							assert( getActiveInstance() != nullptr );
							job->execute();
							if (history_.size() > currentIndex_.value() + 1)
							{
								history_.resize( currentIndex_.value() + 1 );
							}
							CommandInstancePtr history = getActiveInstance();
							popActiveInstance();
							this->addToHistory( history );
						}
						else
						{
							job->execute();
						}
					}
					else
					{
						// TODO: supporting recursive command,
						job->execute();
					}
					collection->pop_front();
				}
				continue;
			}

		case UNDO_REDO_SET_DIRECT:
			{
				setSelectedIndexInternal();
				SetEvent( doneHandles_[ UNDO_REDO_SET_DONE ] );
				continue;
			}

		case CREATE_COMPOUND_COMMAND:
			{
				std::vector< size_t > commandIndices;
				size_t count = pCommandInsList_->size();
				for(int i = 0; i < count; i++)
				{
					const Variant & variant = (*pCommandInsList_)[i].value<const Variant &>();
					auto & findIt =
						std::find( history_.begin(), history_.end(), variant );
					if (findIt == history_.end())
					{
						continue;
					}
					commandIndices.push_back( findIt - history_.begin() );
				}
				if (commandIndices.empty())
				{
					break;
				}
				auto macro = pCommandManager_->getDefManager().createT<CompoundCommand>( false );
				macro->setId( macroName_.c_str() );
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
				currentMacro_ = macro;
				SetEvent( doneHandles_[ CREATE_COMPOUND_COMMAND_DONE] );
				continue;
			}
		case EXIT_COMMAND:
		default:
			continue;
		}

	}

	delete commandQueue_.load();
	for( size_t i = 0; i < handles_.size(); ++i )
	{
		CloseHandle( handles_[ i ] );
	}

	for( size_t i = 0; i < doneHandles_.size(); ++i )
	{
		CloseHandle( doneHandles_[ i ] );
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
	pImpl_ = NULL;
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


ObjectHandle CommandManager::createArguments( const char * cmdArgDefName )
{
	IClassDefinition * def = this->getDefManager().getDefinition( cmdArgDefName );
	assert( def != nullptr);
	return def->create();
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
	for(int i = 0; i < count; i++)
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
	for(int i = 0; i < count; i++)
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
CommandInstancePtr CommandManager::getActiveInstance()
{
	return pImpl_->getActiveInstance();
}

//==============================================================================
void CommandManager::pushActiveInstance()
{
	pImpl_->pushActiveInstance();
}

//==============================================================================
void CommandManager::popActiveInstance()
{
	pImpl_->popActiveInstance();
}

//==============================================================================
void CommandManagerImpl::createCompoundCommand(
	GenericList & commandInstanceList, const char * id )
{
	std::lock_guard< std::mutex > guard( macroCreationLock_ );
	pCommandInsList_ = &commandInstanceList;
	macroName_ = id;
	SetEvent( handles_[ CREATE_COMPOUND_COMMAND ] );
	const int selectedIndex = this->getSelected();
	//auto command = history_[selectedIndex].value<CommandInstancePtr>();

	while( WaitForSingleObject(
		doneHandles_[ CREATE_COMPOUND_COMMAND_DONE ], 100 ) == WAIT_TIMEOUT )
	{
		EventListenerCollection::const_iterator it =
			eventListenerCollection_.begin();
		EventListenerCollection::const_iterator itEnd =
			eventListenerCollection_.end();
		for( ; it != itEnd; ++it )
		{ 
			//(*it)->progressMade( *command.get() );
		}
	}
	assert( currentMacro_ != nullptr );
	macros_.push_back( currentMacro_ );
	currentMacro_ = nullptr;
	pCommandInsList_ = NULL;
	macroName_ = "";
}

//==============================================================================
void CommandManager::createCompoundCommand( GenericList & commandInstanceList, const char * id )
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
		assert( false );
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
