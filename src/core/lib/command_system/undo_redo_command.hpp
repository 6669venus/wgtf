#ifndef UNDO_REDO_COMMAND_HPP
#define UNDO_REDO_COMMAND_HPP

#include "command.hpp"

class ObjectHandle;
class IDataStream;
class CommandManager;
namespace
{
	class CommandManagerImpl;
}

class UndoRedoCommand 
	: public Command
{
public:
	const char * getId() const override;
	ObjectHandle execute( const ObjectHandle & arguments ) const override;
	void undo( IDataStream & stream ) const override;
	void redo( IDataStream & stream ) const override;

private:
	friend CommandManagerImpl;
	UndoRedoCommand( CommandManager * pCommandManager );
	CommandManager * pCommandManager_;
};

#endif//UNDO_REDO_COMMAND_HPP
