#include "set_reflectedproperty_command.hpp"

#include "variant/variant.hpp"
#include "command_system_property_setter.hpp"
#include "reflection/i_object_manager.hpp"
#include "command_system/command_system_provider.hpp"

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
	auto objManager = arguments.getDefinition()->getDefinitionManager()->getObjectManager();
	assert( objManager != nullptr );
	const ObjectHandle & object = objManager->getObject( commandArgs->getContextId() );
	PropertyAccessor property = object.getDefinition()->bindProperty( commandArgs->getPropertyPath(), object );
	if (property.isValid() == false)
	{
		//Can't set
		return nullptr;
	}
	const Variant & data = commandArgs->getPropertyValue();
	if (ReflectionUtilities::isPolyStruct( property ))
	{
		ObjectHandle provider;
		ObjectHandle created;
		if (data.tryCast< ObjectHandle >( provider ))
		{
			auto classDefinition = provider.getBase< IClassDefinition >();
			if (classDefinition != nullptr)
			{
				created = classDefinition->create();
			}
		}
		property.setValue( created );
		return nullptr;
	}



	bool br = property.setValue( data );
	if (!br)
	{
		setErrorCode( NGT_INVALID_VALUE );
	}
	return nullptr;
}