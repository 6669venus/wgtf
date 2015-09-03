#include "macros_object.hpp"
#include "core_command_system/i_command_manager.hpp"
//TODO: remove this pragma
#pragma warning (push)
#pragma warning (disable : 4996 )
#include "core_data_model/generic_list.hpp"
#pragma warning( pop )
#include "core_command_system/macro_object.hpp"
#include "core_command_system/compound_command.hpp"
#include "core_logging/logging.hpp"
#include <cassert>


//==============================================================================
MacrosObject::MacrosObject()
	: commandSystem_( nullptr )
{
}


//==============================================================================
void MacrosObject::init( ICommandManager& commandSystem )
{
	commandSystem_ = &commandSystem;
}


//==============================================================================
ObjectHandle MacrosObject::getMacros() const
{
	assert( commandSystem_ != nullptr );
	return commandSystem_->getMacros();
}


//==============================================================================
ObjectHandle MacrosObject::selectionHandlerSource() const
{
	return ObjectHandle( &selectionHandler );
}


//==============================================================================
ObjectHandle MacrosObject::getSelectedCompoundCommand() const
{
	assert( commandSystem_ != nullptr );
	typedef GenericListT< ObjectHandleT< CompoundCommand > > MacroList;
	const MacroList & macros = static_cast< const MacroList & >( commandSystem_->getMacros() );
	std::vector< int > rows = selectionHandler.getSelectedRows();
	if (rows.empty())
	{
		NGT_ERROR_MSG( "Please select a macro. \n" );
		return nullptr;
	}
	assert( rows.size() == 1 );
	int row = rows[0];
	assert( row >= 0 && row < static_cast<int>( macros.size() ) );
	const ObjectHandleT<CompoundCommand> & macro = macros[row].value();
	if (macro == nullptr)
	{
		NGT_ERROR_MSG( "The macro does not exist. \n" );
		return nullptr;
	}
	return macro;
}
