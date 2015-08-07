#include "set_reflectedpropertyrootobject_command.hpp"

#include "core_variant/variant.hpp"
#include "command_system_property_setter.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "reflectedpropertyrootobject_setter.hpp"

//==============================================================================
const char * ReflectedPropertyRootObjectCommandArgument::s_SourceName = "RootObjectSource";
const char * ReflectedPropertyRootObjectCommandArgument::s_ValueName = "RootObjectValue";

//==============================================================================
const char * ReflectedPropertyRootObjectCommandArgument::sourceName()
{
	return s_SourceName;
}

//==============================================================================
const char * ReflectedPropertyRootObjectCommandArgument::valueName()
{
	return s_ValueName;
}

//==============================================================================
ReflectedPropertyRootObjectCommandArgument::ReflectedPropertyRootObjectCommandArgument()
{
}

//==============================================================================
void ReflectedPropertyRootObjectCommandArgument::init(ReflectedPropertyRootObjectSetter * pSetter, 
													  const ObjectHandle & value)
{
	pSetter_ = pSetter;
	value_ = value;
}

//==============================================================================
const ObjectHandle & ReflectedPropertyRootObjectCommandArgument::getValue() const
{
	return value_;
}

//==============================================================================
void ReflectedPropertyRootObjectCommandArgument::setValue(const ObjectHandle & value)
{
	value_ = value;
}


//==============================================================================
const char * SetReflectedPropertyRootObjectCommand::getId() const
{
	static const char * s_Id = getClassIdentifier<SetReflectedPropertyRootObjectCommand>();
	return s_Id;
}
//==============================================================================
ObjectHandle SetReflectedPropertyRootObjectCommand::execute(
	const ObjectHandle & arguments ) const
{
	auto commandArgs =
		arguments.getBase< ReflectedPropertyRootObjectCommandArgument >();
	auto objManager = arguments.getDefinition()->getDefinitionManager()->getObjectManager();
	assert( objManager != nullptr );
	const ObjectHandle & value = commandArgs->getValue();
	commandArgs->pSetter_->setValue( value );
	return nullptr;
}