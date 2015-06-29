#include "set_reflectedproperty_command.hpp"

#include "variant/variant.hpp"
#include "command_system_property_setter.hpp"
#include "reflection/i_object_manager.hpp"
#include "command_system/i_command_manager.hpp"

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
	// special handle for polystruct
	if (ReflectionUtilities::isPolyStruct( property ))
	{
		ObjectHandle provider;
		
		if (data.tryCast< ObjectHandle >( provider ))
		{
			auto classDefinition = provider.getBase< IClassDefinition >();
			if (classDefinition != nullptr)
			{
				ObjectHandle created;
				created = classDefinition->create();
				property.setValue( created );
				return nullptr;
			}
		}
	}

	// if the value's definition is not matching to 
	// target definition, do not set the value
	auto value = property.getValue();
	ObjectHandle baseProvider;
	value.tryCast( baseProvider );
	if (baseProvider.isValid())
	{
		auto desDef = baseProvider.getDefinition();
		if (desDef != nullptr)
		{
			ObjectHandle provider;
			data.tryCast( provider );
			if (!provider.isValid())
			{
				setErrorCode( NGT_INVALID_VALUE );
				return nullptr;
			}
			auto def = provider.getDefinition();
			if (def == nullptr)
			{
				setErrorCode( NGT_INVALID_VALUE );
				return nullptr;
			}
			// check generic definition
			if (desDef->isGeneric())
			{
				if (!def->isGeneric())
				{
					setErrorCode( NGT_INVALID_VALUE );
					return nullptr;
				}
			}
			else
			{
				if(!def->canBeCastTo( *desDef ))
				{
					setErrorCode( NGT_INVALID_VALUE );
					return nullptr;
				}
			}
			
		}
	}

	bool br = property.setValue( data );
	if (!br)
	{
		setErrorCode( NGT_INVALID_VALUE );
	}
	return nullptr;
}