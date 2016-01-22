#include "compound_command.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "macro_object.hpp"
#include "command_instance.hpp"
#include "batch_command.hpp"
#include "core_reflection/i_definition_manager.hpp"

//==============================================================================
CompoundCommand::CompoundCommand()
	: id_( "" )
	, macroObject_( nullptr )
{

}


//==============================================================================
CompoundCommand::~CompoundCommand()
{
	subCommands_.clear();
}


//==============================================================================
void CompoundCommand::initDisplayData( IDefinitionManager& defManager, IReflectionController* controller )
{
	auto cmdSysProvider = getCommandSystemProvider();
	assert( cmdSysProvider != nullptr );
	const auto pDefinition = defManager.getDefinition(
		getClassIdentifier< MacroObject >() );
	assert( pDefinition != nullptr );

	macroObject_ = defManager.create< MacroObject >( false );
	macroObject_->init( *cmdSysProvider, defManager, controller, id_.c_str() );
}


//==============================================================================
const char * CompoundCommand::getId() const
{
	return id_.c_str();
}


//==============================================================================
void CompoundCommand::addCommand(  const char * commandId, const ObjectHandle & commandArguments )
{
	subCommands_.emplace_back( commandId, commandArguments );
}


//==============================================================================
ObjectHandle CompoundCommand::execute( const ObjectHandle & arguments ) const
{
	auto cmdSysProvider = getCommandSystemProvider();
	assert( cmdSysProvider != nullptr );
	MacroEditObject* ccArgs = arguments.getBase< MacroEditObject >();
	assert( ccArgs );

	for (int i = 0; i < subCommands_.size(); ++i)
	{
		auto instance = cmdSysProvider->queueCommand( subCommands_[i].first.c_str(), ccArgs->getCommandArgument(i) );
		assert( instance != nullptr );
		cmdSysProvider->waitForInstance( instance );
	}
	
	return nullptr;
}


//==============================================================================
CommandThreadAffinity CompoundCommand::threadAffinity() const
{
	return CommandThreadAffinity::ANY_THREAD;
}


//==============================================================================
ObjectHandle CompoundCommand::getMacroObject() const
{
	return macroObject_;
}


//==============================================================================
void CompoundCommand::setId( const char * id )
{
	id_ = id;
}


//==============================================================================
const CompoundCommand::SubCommandCollection & CompoundCommand::getSubCommands() const
{
	return subCommands_;
}

