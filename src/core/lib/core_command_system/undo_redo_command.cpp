#include "undo_redo_command.hpp"
#include "core_reflection/object_handle.hpp"
#include "command_manager.hpp"
#include "core_data_model/generic_list.hpp"
#include "command_instance.hpp"
#include "command_manager.hpp"


//--------------------------------------------------------------------------
UndoRedoCommand::UndoRedoCommand( CommandManager * pCommandManager )
    : pCommandManager_( pCommandManager )
{
}


//--------------------------------------------------------------------------
const char * UndoRedoCommand::getId() const
{
    static const char * s_id = typeid( UndoRedoCommand ).name();
    return s_id;
}


//--------------------------------------------------------------------------
ObjectHandle UndoRedoCommand::execute( const ObjectHandle & arguments ) const
{
	assert( pCommandManager_ != nullptr );
	auto pValue = arguments.getBase<int>();
	assert( pValue != nullptr );
	if (pValue == nullptr)
	{
		return ObjectHandle::makeStorageBackedProvider( CommandErrorCode::INVALID_ARGUMENTS );
	}
	if (!pCommandManager_->undoRedo( *pValue ))
	{
		return ObjectHandle::makeStorageBackedProvider( CommandErrorCode::INVALID_VALUE );
	}
	
	return ObjectHandle::makeStorageBackedProvider( CommandErrorCode::NO_ERROR );
}


//--------------------------------------------------------------------------
void UndoRedoCommand::undo( IDataStream & stream ) const
{
}


//--------------------------------------------------------------------------
void UndoRedoCommand::redo( IDataStream & stream ) const
{
}