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
#include "core_reflection_utils/commands/set_reflectedproperty_command.hpp"
#include "wg_types/binary_block.hpp"
#include "core_logging/logging.hpp"

//==============================================================================
CommandInstance::CommandInstance()
	: defManager_( nullptr )
	, status_( Complete )
	, arguments_( nullptr )
	, pCmdSysProvider_( nullptr )
	, commandId_("")
	, contextObject_( nullptr )
	, errorCode_( CommandErrorCode::COMMAND_NO_ERROR )
{
}

//==============================================================================
CommandInstance::CommandInstance( const CommandInstance& )
{
	assert(!"Not copyable");
}


//==============================================================================
CommandInstance::~CommandInstance()
{
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
        if (childErrorCode == CommandErrorCode::COMMAND_NO_ERROR)
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
	getCommand()->undo( getArguments() );
}


//==============================================================================
void CommandInstance::redo()
{
	getCommand()->redo( getArguments() );
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
    ReflectedPropertyCommandArgument * arguments = getArguments().getBase<ReflectedPropertyCommandArgument>();
    if (arguments != nullptr)
    {
        arguments->connectEvents(defManager_);
    }
}

//==============================================================================
void CommandInstance::disconnectEvent()
{
    ReflectedPropertyCommandArgument * arguments = getArguments().getBase<ReflectedPropertyCommandArgument>();
    if (arguments != nullptr)
    {
        arguments->disconnectEvents(defManager_);
    }
}

//==============================================================================
void CommandInstance::setContextObject( const ObjectHandle & contextObject )
{
	contextObject_ = contextObject;
}

ObjectHandle CommandInstance::getCommandDescription() const
{
    return getCommand()->getCommandDescription(getArguments());
}

//==============================================================================
void CommandInstance::setCommandSystemProvider( ICommandManager * pCmdSysProvider )
{
	pCmdSysProvider_ = pCmdSysProvider;
}
