#include "reflection_controller.hpp"
#include "command_system/i_command_manager.hpp"
#include "commands/set_reflectedproperty_command.hpp"
#include "dependency_system/di_ref.hpp"
#include "reflection/property_accessor.hpp"

class ReflectionController::Impl
{
public:
	Impl( IContextManager & contextManager )
		: commandManager_( contextManager )
	{

	}

	Variant getValue( const PropertyAccessor & pa )
	{
		Key key;
		if (!createKey( pa, key ))
		{
			assert( false );
			return Variant();
		}

		// TODO: assert access is only on the main thread
		auto it = commands_.find( key );
		if (it != commands_.end())
		{
			it->second->waitForCompletion();
			commands_.erase( it );
		}

		return pa.getValue();
	}

	void setValue( const PropertyAccessor & pa, const Variant & data )
	{
		if (commandManager_.get() == nullptr)
		{
			// TODO: error? set pa directly?
			return;
		}

		Key key;
		if (!createKey( pa, key ))
		{
			assert( false );
			return;
		}

		ReflectedPropertyCommandArgument args;
		args.setContextId( key.first );
		args.setPath( key.second.c_str() );
		args.setValue( data );
		
		// TODO: assert access is only on the main thread
		commands_[key] = commandManager_->queueCommand( 
			getClassIdentifier<SetReflectedPropertyCommand>(), args );
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
			return false;
		}

		o_Key.second = pa.getFullPath();
		return true;
	}

	DIRef< ICommandManager > commandManager_;
	std::map< Key, CommandInstancePtr > commands_;
};

ReflectionController::ReflectionController()
{

}

ReflectionController::~ReflectionController()
{

}

void ReflectionController::init( IContextManager & contextManager )
{
	impl_.reset( new Impl( contextManager ) );
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