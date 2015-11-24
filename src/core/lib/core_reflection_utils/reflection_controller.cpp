#include "reflection_controller.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "commands/set_reflectedproperty_command.hpp"
#include "commands/invoke_reflected_method_command.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/reflected_method_parameters.hpp"

class ReflectionController::Impl
{
public:
	Impl( ICommandManager & commandManager )
		: commandManager_( commandManager )
	{

	}

	Variant getValue( const PropertyAccessor & pa )
	{
		Key key;
		if (!createKey( pa, key ))
		{
			return pa.getValue();
		}

		// TODO: assert access is only on the main thread
		auto it = commands_.find( key );
		if (it != commands_.end())
		{
			auto instance = it->second;
			commandManager_.waitForInstance(instance);

			it = commands_.find(key);
			if (it != commands_.end())
			{
				commands_.erase(it);
			}
		}

		return pa.getValue();
	}

	void setValue( const PropertyAccessor & pa, const Variant & data )
	{
		Key key;
		if (!createKey( pa, key ))
		{
			pa.setValue( data );
			return;
		}

		std::unique_ptr< ReflectedPropertyCommandArgument > args( new ReflectedPropertyCommandArgument );
		args->setContextId( key.first );
		args->setPath( key.second.c_str() );
		args->setValue( data );
		
		// TODO: assert access is only on the main thread
		commands_.insert( std::pair< Key, CommandInstancePtr >( key, commandManager_.queueCommand(
			getClassIdentifier<SetReflectedPropertyCommand>(), ObjectHandle( 
				std::move( args ), pa.getDefinitionManager()->getDefinition< ReflectedPropertyCommandArgument >() ) ) ) );
	}

	Variant invoke( const PropertyAccessor & pa, const ReflectedMethodParameters & parameters )
	{
		Key key;

		if (!createKey( pa, key ))
		{
			return pa.invoke( parameters );
		}

		std::unique_ptr<ReflectedMethodCommandParameters> commandParameters( new ReflectedMethodCommandParameters() );
		commandParameters->setId( key.first );
		commandParameters->setPath( key.second.c_str() );
		commandParameters->setParameters( parameters );

		commands_.insert( std::pair< Key, CommandInstancePtr >( key, commandManager_.queueCommand(
			getClassIdentifier<InvokeReflectedMethodCommand>(), ObjectHandle( std::move( commandParameters ),
			pa.getDefinitionManager()->getDefinition<ReflectedMethodCommandParameters>() ) ) ) );

		commandManager_.waitForInstance( commands_[key] );
		ObjectHandle returnValueObject = commands_[key].get()->getReturnValue();
		commands_.erase( commands_.find( key ) );
		Variant* returnValuePointer = returnValueObject.getBase<Variant>();
		assert( returnValuePointer != nullptr );
		return *returnValuePointer;
	}

private:
	typedef std::pair< RefObjectId, std::string > Key;
	bool createKey( const PropertyAccessor & pa, Key & o_Key )
	{
		const auto & obj = pa.getRootObject();
		if (obj == nullptr)
		{
			return false;
		}

		if (!obj.getId( o_Key.first ))
		{
			auto om = pa.getDefinitionManager()->getObjectManager();
			assert( !om->getObject( obj.data() ).isValid() );
			if (!om->getUnmanagedObjectId( obj.data(), o_Key.first ))
			{
				o_Key.first = om->registerUnmanagedObject( obj );
			}
		}

		o_Key.second = pa.getFullPath();
		return true;
	}

	ICommandManager & commandManager_;
	std::map< Key, CommandInstancePtr > commands_;
};

ReflectionController::ReflectionController()
{

}

ReflectionController::~ReflectionController()
{

}

void ReflectionController::init( ICommandManager & commandManager )
{
	impl_.reset( new Impl( commandManager ) );
}

Variant ReflectionController::getValue( const PropertyAccessor & pa )
{
	assert( impl_ != nullptr );
	return impl_->getValue( pa );
}

void ReflectionController::setValue( const PropertyAccessor & pa, const Variant & data )
{
	assert( impl_ != nullptr );
	impl_->setValue( pa, data );
}

Variant ReflectionController::invoke( const PropertyAccessor & pa, const ReflectedMethodParameters & parameters )
{
	assert( impl_ != nullptr );
	return impl_->invoke( pa, parameters );
}
