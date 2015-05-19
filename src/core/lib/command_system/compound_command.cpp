#include "compound_command.hpp"
#include "command_system/command_system_provider.hpp"
#include "macro_object.hpp"
#include "command_instance.hpp"

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

	macroObject_ = pDefinition->create();
	macroObject_->init( *cmdSysProvider, defManager, id_.c_str() );
}


//==============================================================================
const char * CompoundCommand::getId() const
{
	return id_.c_str();
}


//==============================================================================
void CompoundCommand::addCommand( const IDefinitionManager & defManager, const CommandInstancePtr & commandInstance )
{
	// always create a new CommandInstance to hold a copy of history data
	auto instance = defManager.createT< CommandInstance >( false );
	instance->setCommandSystemProvider( getCommandSystemProvider() );
	instance->setCommandId( id_.c_str() );
	instance->init( std::this_thread::get_id() );

	// copy the redo history data to new created CommandInstance
	std::string redoData;
	commandInstance->getRedoData( &redoData );
	instance->setRedoData( redoData );
	subCommands_.push_back( instance );
}


//==============================================================================
ObjectHandle CompoundCommand::execute( const ObjectHandle & arguments ) const
{
	auto commandArgs =
		arguments.getBase< CompoundCommandArgument >();
	assert( commandArgs != nullptr );

	const ObjectHandle & contextObject = commandArgs->getContextObject();
	SubCommandCollection::const_iterator it = subCommands_.begin();
	SubCommandCollection::const_iterator itEnd = subCommands_.end();

	// Let listeners know multi command has started
	if ( nullptr != getCommandSystemProvider() )
	{
		getCommandSystemProvider()->notifyBeginMultiCommand();
	}

	for( ; it != itEnd; ++it )
	{
		(*it)->setContextObject( contextObject );
		(*it)->redo();
		(*it)->setContextObject( nullptr );
	}
	
	// Let listeners know multi command has completed
	if ( nullptr != getCommandSystemProvider() )
	{
		getCommandSystemProvider()->notifyCompleteMultiCommand();
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

