#include "command.hpp"
#include "command_system_provider.hpp"

//==============================================================================
Command::~Command()
{
	eventListenerCollection_.clear();
	commandSystemProvider_ = nullptr;
}

//==============================================================================
void Command::registerCommandStatusListener(
	ICommandEventListener * listener )
{
	eventListenerCollection_.push_back( listener );
}


//==============================================================================
void Command::setCommandSystemProvider(
	ICommandManager * commandSystemProvider )
{
	commandSystemProvider_ = commandSystemProvider;
}


//==============================================================================
ICommandManager * Command::getCommandSystemProvider() const
{
	return commandSystemProvider_;
}


//==============================================================================
void Command::fireCommandStatusChanged(
	const CommandInstance & command ) const
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
void Command::fireProgressMade( const CommandInstance & command ) const
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
/*virtual */ObjectHandle Command::execute(
	const ObjectHandle & arguments ) const
{
	return nullptr;
}

void Command::setErrorCode( NGTCommandErrorCode errorCode ) const
{
	assert( commandSystemProvider_ != nullptr );
	commandSystemProvider_->setErrorCode( errorCode );
}