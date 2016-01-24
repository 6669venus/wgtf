#include "set_reflectedproperty_command.hpp"

#include "core_variant/variant.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_reflection_utils/commands/reflectedproperty_undoredo_helper.hpp"

namespace RPURU = ReflectedPropertyUndoRedoUtility;

//==============================================================================
const char * ReflectedPropertyCommandArgument::s_ContextId = "PropertyContextId";
const char * ReflectedPropertyCommandArgument::s_PropertyPath = "PropertyPath";
const char * ReflectedPropertyCommandArgument::s_PropertyValue = "PropertyValue";


//==============================================================================
const char * ReflectedPropertyCommandArgument::contextIdPropertyName()
{
	return s_ContextId;
}


//==============================================================================
const char * ReflectedPropertyCommandArgument::pathPropertyName()
{
	return s_PropertyPath;
}


//==============================================================================
const char * ReflectedPropertyCommandArgument::valuePropertyName()
{
	return s_PropertyValue;
}

//==============================================================================
ReflectedPropertyCommandArgument::ReflectedPropertyCommandArgument()
	: contextId_("")
	, propertyPath_("")
{
}


//==============================================================================
const RefObjectId & ReflectedPropertyCommandArgument::getContextId() const
{
	return contextId_;
}


//==============================================================================
const char * ReflectedPropertyCommandArgument::getPropertyPath() const
{
	return propertyPath_.c_str();
}


//==============================================================================
const Variant & ReflectedPropertyCommandArgument::getPropertyValue() const
{
	return value_;
}


//==============================================================================
void ReflectedPropertyCommandArgument::setContextId( const RefObjectId & id )
{
	contextId_ = id;
}


//==============================================================================
void ReflectedPropertyCommandArgument::setPath( const char * path )
{
	propertyPath_ = path;
}


//==============================================================================
void ReflectedPropertyCommandArgument::setValue( const Variant & value )
{
	value_ = value;
}


//==============================================================================
SetReflectedPropertyCommand::SetReflectedPropertyCommand( IDefinitionManager & definitionManager )
	: definitionManager_( definitionManager )
{
}


//==============================================================================
SetReflectedPropertyCommand::~SetReflectedPropertyCommand()
{
}


//==============================================================================
const char * SetReflectedPropertyCommand::getId() const
{
	static const char * s_Id = getClassIdentifier<SetReflectedPropertyCommand>();
	return s_Id;
}


//==============================================================================
ObjectHandle SetReflectedPropertyCommand::execute(
	const ObjectHandle & arguments ) const
{
	auto commandArgs =
		arguments.getBase< ReflectedPropertyCommandArgument >();
	auto objManager = definitionManager_.getObjectManager();
	assert( objManager != nullptr );
	const ObjectHandle & object = objManager->getObject( commandArgs->getContextId() );
	PropertyAccessor property = object.getDefinition( definitionManager_ )->bindProperty( 
		commandArgs->getPropertyPath(), object );
	if (property.isValid() == false)
	{
		//Can't set
		return CommandErrorCode::INVALID_ARGUMENTS;
	}
	const Variant & data = commandArgs->getPropertyValue();
	bool br = property.setValue( data );
	if (!br)
	{
		return CommandErrorCode::INVALID_VALUE;
	}
	return nullptr;
}


//==============================================================================
CommandThreadAffinity SetReflectedPropertyCommand::threadAffinity() const
{
	return CommandThreadAffinity::UI_THREAD;
}

typedef XMLSerializer UndoRedoSerializer;

void SetReflectedPropertyCommand::undo(IDataStream & dataStore) const 
{
}

void SetReflectedPropertyCommand::redo(IDataStream & dataStore) const 
{
}
