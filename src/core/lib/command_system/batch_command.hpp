#ifndef BATCH_COMMAND_HPP
#define BATCH_COMMAND_HPP

#include "command.hpp"

class ObjectHandle;
class IDataStream;
class CommandManager;

class BeginBatchCommand
    : public Command
{
public:
    BeginBatchCommand( CommandManager * pCommandManager );

    const char * getId() const override;
    ObjectHandle execute( const ObjectHandle & arguments ) const override;
    void undo( IDataStream & stream ) const override;
    void redo( IDataStream & stream ) const override;

private:
    CommandManager * pCommandManager_;
};


class EndBatchCommand
    : public Command
{
public:
    EndBatchCommand( CommandManager * pCommandManager );

    const char * getId() const override;
    ObjectHandle execute( const ObjectHandle & arguments ) const override;
    void undo( IDataStream & stream ) const override;
    void redo( IDataStream & stream ) const override;

private:
    CommandManager * pCommandManager_;
};


class AbortBatchCommand
    : public Command
{
public:
    AbortBatchCommand( CommandManager * pCommandManager );

    const char * getId() const override;
    ObjectHandle execute( const ObjectHandle & arguments ) const override;
    void undo( IDataStream & stream ) const override;
    void redo( IDataStream & stream ) const override;

private:
    CommandManager * pCommandManager_;
};

#endif//BATCH_COMMAND_HPP
