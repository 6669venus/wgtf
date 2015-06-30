#include "batch_command.hpp"
#include "reflection/object_handle.hpp"
#include "command_manager.hpp"


//--------------------------------------------------------------------------
BeginBatchCommand::BeginBatchCommand( CommandManager * pCommandManager )
    : pCommandManager_( pCommandManager )
{
}


//--------------------------------------------------------------------------
const char * BeginBatchCommand::getId() const
{
    static const char * s_id = typeid( BeginBatchCommand ).name();
    return s_id;
}


//--------------------------------------------------------------------------
ObjectHandle BeginBatchCommand::execute( const ObjectHandle & arguments ) const
{
    assert( pCommandManager_ != nullptr );
    pCommandManager_->notifyBeginMultiCommand();
    pCommandManager_->pushActiveInstance( nullptr );
    return nullptr;
}


//--------------------------------------------------------------------------
void BeginBatchCommand::undo( IDataStream & stream ) const
{
}


//--------------------------------------------------------------------------
void BeginBatchCommand::redo( IDataStream & stream ) const
{
}


//--------------------------------------------------------------------------
EndBatchCommand::EndBatchCommand( CommandManager * pCommandManager )
    : pCommandManager_( pCommandManager )
{
}

//--------------------------------------------------------------------------
const char * EndBatchCommand::getId() const
{
    static const char * s_id = typeid( EndBatchCommand ).name();
    return s_id;
}


//--------------------------------------------------------------------------
ObjectHandle EndBatchCommand::execute( const ObjectHandle & arguments ) const
{
    assert( pCommandManager_ != nullptr );
    pCommandManager_->popActiveInstance();
    pCommandManager_->notifyCompleteMultiCommand();
    return nullptr;
}


//--------------------------------------------------------------------------
void EndBatchCommand::undo( IDataStream & stream ) const
{
}


//--------------------------------------------------------------------------
void EndBatchCommand::redo( IDataStream & stream ) const
{
}


//--------------------------------------------------------------------------
AbortBatchCommand::AbortBatchCommand( CommandManager * pCommandManager )
    : pCommandManager_( pCommandManager )
{
}

//--------------------------------------------------------------------------
const char * AbortBatchCommand::getId() const
{
    static const char * s_id = typeid( AbortBatchCommand ).name();
    return s_id;
}


//--------------------------------------------------------------------------
ObjectHandle AbortBatchCommand::execute( const ObjectHandle & arguments ) const
{
    assert( pCommandManager_ != nullptr );
    pCommandManager_->popActiveInstance();
    pCommandManager_->setErrorCode( NGT_ABORTED );
    pCommandManager_->notifyCancelMultiCommand();
    return nullptr;
}


//--------------------------------------------------------------------------
void AbortBatchCommand::undo( IDataStream & stream ) const
{
}


//--------------------------------------------------------------------------
void AbortBatchCommand::redo( IDataStream & stream ) const
{
}

