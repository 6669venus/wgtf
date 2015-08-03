#include "command_system_property_setter.hpp"

#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/reflected_object.hpp"

#include "core_command_system/i_command_manager.hpp"

#include "commands/set_reflectedproperty_command.hpp"
#include "core_variant/variant.hpp"

#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/i_object_manager.hpp"

CommandSystemReflectionPropertySetter::CommandSystemReflectionPropertySetter()
	: commandSystemProvider_( nullptr )
{

}


//==============================================================================
void CommandSystemReflectionPropertySetter::init( 
	ICommandManager & commandSystemProvider )
{
	commandSystemProvider_ = &commandSystemProvider;
}


//==============================================================================
void CommandSystemReflectionPropertySetter::setDataValue(
	const PropertyAccessor & pa, const Variant & data )
{
	assert( commandSystemProvider_ );

	const auto & obj = pa.getRootObject();
	assert( obj != nullptr );
	RefObjectId id;
	
	if (!obj.getId( id ))
	{
		auto om = pa.getDefinitionManager()->getObjectManager();
		assert( !om->getObject( obj.getStorage()->getRaw() ).isValid() );
		ObjectHandle oh = om->getUnmanagedObject( obj.getStorage()->getRaw() );
		if (!oh.isValid())
		{
			id = om->registerUnmanagedObject(const_cast<ObjectHandle&>(obj));
		}
		else
		{
			bool ok = oh.getId(id);
			assert( ok );
		}
	}

	const char * propertyPath = pa.getFullPath();
	const TypeId type = pa.getType();
	auto argDef = pa.getDefinitionManager()->getDefinition<ReflectedPropertyCommandArgument>();
	assert( argDef != nullptr );
	ObjectHandleT<ReflectedPropertyCommandArgument> arguments = argDef->create();
	assert( arguments != nullptr );
	arguments->setContextId( id );
	arguments->setPath( propertyPath );
	arguments->setValue( data );
	commandSystemProvider_->queueCommand( getClassIdentifier<SetReflectedPropertyCommand>(), ObjectHandle( arguments ) );

	// Notify any listener about this non-blocking process
	commandSystemProvider_->notifyNonBlockingProcessExecution( propertyPath );
}