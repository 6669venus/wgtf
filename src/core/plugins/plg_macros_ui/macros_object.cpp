#include "macros_object.hpp"
#include "command_system/command_system_provider.hpp"
#include "data_model/generic_list.hpp"
#include "command_system/macro_object.hpp"
#include "command_system/compound_command.hpp"
#include "logging/logging.hpp"
#include <cassert>


//==============================================================================
MacrosObject::MacrosObject()
	: commandSystem_( nullptr )
	, currentSelectedRowIndex_( -1 )
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
const int & MacrosObject::currentSelectedRowIndex() const
{
	return currentSelectedRowIndex_;
}


//==============================================================================
void MacrosObject::currentSelectedRowIndex( const int & index )
{
	currentSelectedRowIndex_ = index;
}


//==============================================================================
ObjectHandle MacrosObject::getSelectedCompoundCommand() const
{
	assert( commandSystem_ != nullptr );
	const GenericList & macros = commandSystem_->getMacros();
	if ((currentSelectedRowIndex_ < 0) || (currentSelectedRowIndex_ >= static_cast<int>(macros.size())))
	{
		NGT_ERROR_MSG( "Please select a macro. \n" );
		return nullptr;
	}

	const Variant & variant = macros[currentSelectedRowIndex_].value<const Variant &>();
	ObjectHandleT<CompoundCommand> macro;
	bool isOk = variant.tryCast( macro );
	assert(isOk);
	if (macro == nullptr)
	{
		NGT_ERROR_MSG( "The macro does not exist. \n" );
		return nullptr;
	}
	return macro;
}
