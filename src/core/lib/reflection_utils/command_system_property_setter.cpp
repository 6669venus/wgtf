#include "command_system_property_setter.hpp"

#include "reflection/interfaces/i_class_definition.hpp"
#include "reflection/property_accessor.hpp"
#include "reflection/reflected_object.hpp"

#include "command_system/command_system_provider.hpp"

#include "commands/set_reflectedproperty_command.hpp"
#include "variant/variant.hpp"

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
	bool ok = obj.getId( id );
	assert( ok );
	const char * propertyPath = pa.getFullPath();
	const TypeId type = pa.getType();
	ObjectHandleT<ReflectedPropertyCommandArgument> arguments = 
		commandSystemProvider_->createCommandArgumentT<ReflectedPropertyCommandArgument>();
	assert( arguments != nullptr );
	arguments->setContextId( id );
	arguments->setPath( propertyPath );
	arguments->setValue( data );
	commandSystemProvider_->queueCommand( getClassIdentifier<SetReflectedPropertyCommand>(), ObjectHandle( arguments ) );

	// Notify any listener about this non-blocking process
	commandSystemProvider_->notifyNonBlockingProcessExecution( propertyPath );
}