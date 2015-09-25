#include "command_instance.hpp"
#include "command.hpp"
#include "i_command_manager.hpp"

#include "core_data_model/collection_model.hpp"

#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/property_iterator.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"

#include "core_logging/logging.hpp"


namespace RPURU = ReflectedPropertyUndoRedoUtility;
namespace
{

	//==========================================================================
	class PropertyAccessorWrapper
		: public PropertyAccessorListener
	{
	public:
		PropertyAccessorWrapper(
			RPURU::UndoRedoHelperList & undoRedoHelperList,
			const std::thread::id& commandThreadId )
			: undoRedoHelperList_( undoRedoHelperList ) 
			, commandThreadId_( commandThreadId )
		{
		}

		//======================================================================
		void preSetValue(
			const PropertyAccessor & accessor, const Variant & value ) override
		{
			assert( (std::this_thread::get_id() == commandThreadId_) &&
				"To record undo/redo data, properties must be changed using"
				"a command on the command thread" );

			const auto & obj = accessor.getRootObject();
			assert( obj != nullptr );
			RefObjectId id;
			bool ok = obj.getId( id );
			assert( ok );
			const char * propertyPath = accessor.getFullPath();
			const TypeId type = accessor.getType();
			Variant prevalue = accessor.getValue();
			auto pHelper = this->findUndoRedoHelper( id, propertyPath );
			if (pHelper != nullptr)
			{
				return;
			}
			auto helper = new RPURU::ReflectedPropertyUndoRedoHelper();
			helper->objectId_ = id;
			helper->path_ = propertyPath;
			helper->typeName_ = type.getName();
			helper->preValue_ = std::move( prevalue );
			undoRedoHelperList_.emplace_back( helper );
		}


		//======================================================================
		void postSetValue(
			const PropertyAccessor & accessor, const Variant & value ) override
		{
			assert( (std::this_thread::get_id() == commandThreadId_) &&
				"To record undo/redo data, properties must be changed using"
				"a command on the command thread" );

			const auto & obj = accessor.getRootObject();
			assert( obj != nullptr );
			RefObjectId id;
			bool ok = obj.getId( id );
			assert( ok );
			const char * propertyPath = accessor.getFullPath();
			 Variant postValue = accessor.getValue();
			RPURU::ReflectedPropertyUndoRedoHelper* pHelper = static_cast<RPURU::ReflectedPropertyUndoRedoHelper*>(
				this->findUndoRedoHelper( id, propertyPath ) );
			assert( pHelper != nullptr );
			pHelper->postValue_ = std::move( postValue );
		}


		void preInvoke(
			const PropertyAccessor & accessor, const ReflectedMethodParameters& parameters, bool undo ) override
		{
			assert( (std::this_thread::get_id() == commandThreadId_) &&
				"To record undo/redo data, methods must be invoked using"
				"a command on the command thread" );

			const char* path = accessor.getFullPath();
			const auto& object = accessor.getRootObject();
			assert( object != nullptr );

			RefObjectId id;
			assert( object.getId( id ) );

			RPURU::ReflectedMethodUndoRedoHelper* helper = static_cast<RPURU::ReflectedMethodUndoRedoHelper*>(
				this->findUndoRedoHelper( id, path ) );

			if (helper == nullptr)
			{
				auto helper = new RPURU::ReflectedMethodUndoRedoHelper();
				helper->objectId_ = id;
				helper->path_ = path;
				helper->parameters_ = parameters;
				undoRedoHelperList_.emplace_back( helper );
			}
			else
			{
				helper->parameters_ = parameters;
			}
		}


	private:
		RPURU::ReflectedClassMemberUndoRedoHelper* findUndoRedoHelper( 
			const RefObjectId & id, const char * propertyPath )
		{
			RPURU::ReflectedClassMemberUndoRedoHelper* helper = nullptr;
			for (auto& findIt : undoRedoHelperList_)
			{
				if (findIt->objectId_ == id && findIt->path_ == propertyPath)
				{
					helper = findIt.get();
					break;
				}
			}
			return helper;
		}
	private:
		RPURU::UndoRedoHelperList &	undoRedoHelperList_;
		const std::thread::id& commandThreadId_;
	};

}

//==============================================================================
CommandInstance::CommandInstance()
	: defManager_( nullptr )
	, status_( Complete )
	, arguments_( nullptr )
	, pCmdSysProvider_( nullptr )
	, commandId_("")
	, contextObject_( nullptr )
	, errorCode_( CommandErrorCode::NO_ERROR )
{
}

//==============================================================================
CommandInstance::CommandInstance( const CommandInstance& )
{
	assert(!"Not copyable");
}

//==============================================================================
/*virtual */void CommandInstance::init( const std::thread::id& commandThreadId )
{
	paListener_ = std::make_shared< PropertyAccessorWrapper >(
		undoRedoHelperList_,
		commandThreadId );
}


//==============================================================================
CommandInstance::~CommandInstance()
{
	assert( undoRedoHelperList_.empty() );
	paListener_ = nullptr;
}


//==============================================================================
void CommandInstance::cancel()
{
}


//==============================================================================
void CommandInstance::waitForCompletion()
{
	std::unique_lock<std::mutex> lock( mutex_ );

	while( !completeStatus_.wait_for(
		lock,
		std::chrono::milliseconds( 1 ),
		[this] { return status_ == Complete; } ) )
	{
		lock.unlock();
		getCommand()->fireProgressMade( *this );
		lock.lock();
	}
}


//==============================================================================
CommandErrorCode CommandInstance::getErrorCode() const
{
	if (children_.empty())
	{
		return errorCode_;
	}

	//batchcommand(parent command) failure only when:
	//all sub-commands failed(errorcode != NO_ERROR) or as long as one of sub-commands' errorcode == ABORTED
	CommandErrorCode errorCode = CommandErrorCode::ABORTED;
	for (const auto & child : children_)
	{
		CommandErrorCode childErrorCode = child->getErrorCode();
		if (childErrorCode == CommandErrorCode::NO_ERROR)
		{
			errorCode = childErrorCode;
			continue;
		}
		if (childErrorCode == CommandErrorCode::ABORTED)
		{
			errorCode = childErrorCode;
			break;
		}
	}
	return errorCode;
}

//==============================================================================
bool CommandInstance::isMultiCommand() const
{
	return !children_.empty();
}

//==============================================================================
void CommandInstance::setArguments( const ObjectHandle & arguments )
{
	arguments_ = arguments;
}

//==============================================================================
void CommandInstance::setDefinitionManager( IDefinitionManager & defManager )
{
	defManager_ = &defManager;
}

//==============================================================================
const char * CommandInstance::getCommandId() const
{
	return commandId_.c_str();
}


//==============================================================================
void CommandInstance::setCommandId( const char * commandName )
{
	commandId_ = commandName;
}

//==============================================================================
Command * CommandInstance::getCommand()
{
	assert( pCmdSysProvider_ != nullptr );
	Command * pCommand = pCmdSysProvider_->findCommand( commandId_.c_str() );
	assert( pCommand != nullptr );
	return pCommand;
}


const Command * CommandInstance::getCommand() const
{
	assert( pCmdSysProvider_ != nullptr );
	Command * pCommand = pCmdSysProvider_->findCommand( commandId_.c_str() );
	assert( pCommand != nullptr );
	return pCommand;
}

//==============================================================================
/*virtual */void CommandInstance::setStatus( ExecutionStatus status )
{
	std::unique_lock<std::mutex> lock( mutex_ );

	status_ = status;
	getCommand()->fireCommandStatusChanged( *this );
	if (status_ == Complete)
	{
		completeStatus_.notify_all();
	}
}

//==============================================================================
void CommandInstance::undo()
{
	assert( defManager_ != nullptr );
	const auto pObjectManager = defManager_->getObjectManager();
	assert( pObjectManager != nullptr );
	{
		undoData_.seek( 0 );
		UndoRedoSerializer serializer( undoData_, *defManager_ );
		RPURU::performReflectedUndo( serializer, *pObjectManager, *defManager_ );
	}
	getCommand()->undo( undoData_ );
}


//==============================================================================
void CommandInstance::redo()
{
	assert( defManager_ != nullptr );
	const auto pObjectManager = defManager_->getObjectManager();
	assert( pObjectManager != nullptr );
	{
		redoData_.seek( 0 );
		UndoRedoSerializer serializer( redoData_, *defManager_ );
		RPURU::performReflectedRedo( serializer, *pObjectManager, *defManager_ );
	}
	getCommand()->redo( redoData_ );
}


//==============================================================================
void CommandInstance::execute()
{
	returnValue_ = getCommand()->execute( arguments_ );
	auto errorCode = returnValue_.getBase<CommandErrorCode>();
	if (errorCode != nullptr)
	{
		errorCode_ = *errorCode;
	}
}

//==============================================================================
void CommandInstance::connectEvent()
{
	assert( paListener_ );
	assert( defManager_ != nullptr );
	defManager_->registerPropertyAccessorListener( paListener_ );
}

//==============================================================================
void CommandInstance::disconnectEvent()
{
	assert( paListener_ );
	assert( defManager_ != nullptr );
	defManager_->deregisterPropertyAccessorListener( paListener_ );

	UndoRedoSerializer undoSerializer( undoData_, *defManager_ );
	undoSerializer.serialize( RPURU::getUndoStreamHeaderTag() );
	undoSerializer.serialize( undoRedoHelperList_.size() );

	UndoRedoSerializer redoSerializer( redoData_, *defManager_ );
	redoSerializer.serialize( RPURU::getRedoStreamHeaderTag() );
	redoSerializer.serialize( undoRedoHelperList_.size() );

	for (const auto& helper : undoRedoHelperList_)
	{
		RPURU::saveUndoData( undoSerializer, *helper );
		RPURU::saveRedoData( redoSerializer, *helper );
	}

	undoRedoHelperList_.clear();
}

//==============================================================================
void CommandInstance::getUndoData(std::string * undoData) const
{
	*undoData = undoData_.buffer();
}

//==============================================================================
void CommandInstance::setUndoData( const std::string & undoData )
{
	undoData_.setBuffer( undoData );
}

//==============================================================================
void CommandInstance::getRedoData(std::string * redoData) const
{
	*redoData = redoData_.buffer();
}

//==============================================================================
void CommandInstance::setRedoData( const std::string & redoData )
{
	redoData_.setBuffer( redoData );
}

//==============================================================================
void CommandInstance::setContextObject( const ObjectHandle & contextObject )
{
	contextObject_ = contextObject;
}

//==============================================================================
void CommandInstance::setCommandSystemProvider( ICommandManager * pCmdSysProvider )
{
	pCmdSysProvider_ = pCmdSysProvider;
}
