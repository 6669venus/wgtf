#include "history_object.hpp"
#include "command_system/i_command_manager.hpp"
#include "data_model/generic_list.hpp"
#include "logging/logging.hpp"
#include <cassert>


HistoryObject::HistoryObject()
	: commandSystem_( nullptr )
	, currentSelectedRowIndex_( -1 )
{
}


void HistoryObject::init( ICommandManager& commandSystem )
{
	commandSystem_ = &commandSystem;
}


ObjectHandle HistoryObject::getHistory() const
{
	assert( commandSystem_ != nullptr );
	return commandSystem_->getHistory();
}


ObjectHandle HistoryObject::currentIndexSource() const
{
	return ObjectHandle( &commandSystem_->currentIndex() );
}

const int & HistoryObject::currentSelectedRowIndex() const
{
	return currentSelectedRowIndex_;
}
void HistoryObject::currentSelectedRowIndex( const int & index )
{
	currentSelectedRowIndex_ = index;
}

ObjectHandle HistoryObject::createMacro() const
{
	//TODO: http://jira.bigworldtech.com/browse/NGT-392
	assert( commandSystem_ != nullptr );
	if(currentSelectedRowIndex_ == -1)
	{
		NGT_ERROR_MSG( "Please select a command history. \n" );
		return nullptr;
	}
	const GenericList & history = commandSystem_->getHistory();
	GenericList commandList;
	commandList.push_back(history[currentSelectedRowIndex_].value<const Variant &>());
	commandSystem_->createCompoundCommand( commandList );
	return nullptr;
}

