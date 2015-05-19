#ifndef REFLECTED_COMMAND_HPP
#define REFLECTED_COMMAND_HPP

#include <list>

#include "i_command_event_listener.hpp"
#include "command_system/command_instance.hpp"
class IDataStream;
class CommandSystemProvider;


/**
 *	Command specifies the type of actions to be performed.
 */
class Command
{

public:
	virtual ~Command();
	virtual const char * getId() const = 0;
	virtual ObjectHandle execute(
		const ObjectHandle & arguments ) const = 0;

	/**
	 *	Override this method if custom display is needed.
	 *	@param outPropertyCache fill with custom properties.
	 *	@param undoData previous values.
	 *	@param redoData next values.
	 *	@return success.
	 */
	virtual bool createDisplayData(
		UndoRedoHelperList& outPropertyCache,
		const IDataStream& undoData,
		const IDataStream& redoData ) const { return true; }
	virtual void undo( IDataStream & dataStore ) const {}
	virtual void redo( IDataStream & dataStore ) const {}

	void setCommandSystemProvider(
		CommandSystemProvider * commandSystemProvider );
	void registerCommandStatusListener(
		ICommandEventListener * listener );

	CommandSystemProvider * getCommandSystemProvider() const;

	void fireCommandStatusChanged( const CommandInstance & command ) const;
	void fireProgressMade( const CommandInstance & command ) const;

private:
	typedef std::list< ICommandEventListener * > EventListenerCollection;
	EventListenerCollection eventListenerCollection_;
	CommandSystemProvider * commandSystemProvider_;
};

#endif //REFLECTED_COMMAND_HPP