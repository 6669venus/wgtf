#ifndef BATCH_COMMAND_HPP
#define BATCH_COMMAND_HPP

#include "command.hpp"

class ObjectHandle;
class IDataStream;
class CommandManager;

enum class BatchCommandStage : uint8_t
{
	Begin = 0,
	End = 1,
	Abort = 2,
};

class BatchCommand
    : public Command
{
public:
    BatchCommand( CommandManager * pCommandManager );

    const char * getId() const override;
    ObjectHandle execute( const ObjectHandle & arguments ) const override;
    void undo( IDataStream & stream ) const override;
    void redo( IDataStream & stream ) const override;

private:
    CommandManager * pCommandManager_;
};

#endif//BATCH_COMMAND_HPP
