#include "compound_command.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "macro_object.hpp"
#include "command_instance.hpp"
#include "batch_command.hpp"
#include "core_reflection/i_definition_manager.hpp"

//==============================================================================
const char * CompoundCommandArgument::s_ContextObjectPropertyName = "PropertyContextId";


//==============================================================================
const char * CompoundCommandArgument::contextObjectPropertyName()
{
	return s_ContextObjectPropertyName;
}


//==============================================================================
const ObjectHandle & CompoundCommandArgument::getContextObject() const
{
	return contextObject_;
}


//==============================================================================
void CompoundCommandArgument::setContextObject( const ObjectHandle & contextObject )
{
	contextObject_ = contextObject;
}


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
void CompoundCommand::initDisplayData( IDefinitionManager & defManager )
{
	auto cmdSysProvider = getCommandSystemProvider();
	assert( cmdSysProvider != nullptr );
	const auto pDefinition = defManager.getDefinition(
		getClassIdentifier< MacroObject >() );
	assert( pDefinition != nullptr );

	macroObject_ = defManager.create< MacroObject >( false );
	macroObject_->init( *cmdSysProvider, defManager, id_.c_str() );
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

	SubCommandCollection::const_iterator it = subCommands_.begin();
	SubCommandCollection::const_iterator itEnd = subCommands_.end();

	for( ; it != itEnd; ++it )
	{
		auto instance = cmdSysProvider->queueCommand( it->first.c_str(), it->second );
		assert( instance != nullptr );
		cmdSysProvider->waitForInstance( instance );
	}
	
	return nullptr;
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

