#include "batch_command.hpp"
#include "reflection/object_handle.hpp"
#include "command_manager.hpp"


//--------------------------------------------------------------------------
BatchCommand::BatchCommand( CommandManager * pCommandManager )
    : pCommandManager_( pCommandManager )
{
}


//--------------------------------------------------------------------------
const char * BatchCommand::getId() const
{
    static const char * s_id = typeid( BatchCommand ).name();
    return s_id;
}


//--------------------------------------------------------------------------
ObjectHandle BatchCommand::execute( const ObjectHandle & arguments ) const
{
    assert( pCommandManager_ != nullptr );
	auto stage = arguments.getBase<BatchCommandStage>();
	assert( stage != nullptr );
	switch( *stage )
	{
	case BatchCommandStage::Begin:
		pCommandManager_->notifyBeginMultiCommand();
		pCommandManager_->pushBatchCommand();
		break;
	case BatchCommandStage::End:
		pCommandManager_->popBatchCommand();
		break;
	case BatchCommandStage::Abort:
		pCommandManager_->popBatchCommand();
		return ObjectHandle::makeStorageBackedProvider( CommandErrorCode::ABORTED );
	default:
		assert( false );
		break;
	}
    return nullptr;
}


//--------------------------------------------------------------------------
void BatchCommand::undo( IDataStream & stream ) const
{
}


//--------------------------------------------------------------------------
void BatchCommand::redo( IDataStream & stream ) const
{
}
