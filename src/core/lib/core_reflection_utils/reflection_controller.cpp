#include "reflection_controller.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "commands/set_reflectedproperty_command.hpp"
#include "commands/set_reflectedpropertyrootobject_command.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "reflectedpropertyrootobject_setter.hpp"

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

	ObjectHandle getValue( const ReflectedPropertyRootObjectSetter & setter )
	{
		Key key;
		if (!createKey(setter, key))
		{
			return setter.getValue();
		}

		// TODO: assert access is only on the main thread
		auto it = commands_.find(key);
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

		return setter.getValue();
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
		commands_[key] = commandManager_.queueCommand( 
			getClassIdentifier<SetReflectedPropertyCommand>(), ObjectHandle( 
				std::move( args ), pa.getDefinitionManager()->getDefinition< ReflectedPropertyCommandArgument >() ) );
	}

	void setValue( ReflectedPropertyRootObjectSetter & setter, const ObjectHandle & data )
	{
		Key key;
		if (!createKey( setter, key ))
		{
			setter.setValue( data );
			return;
		}

		std::unique_ptr< ReflectedPropertyRootObjectCommandArgument > args(new ReflectedPropertyRootObjectCommandArgument);
		args->init(&setter, data);

		// TODO: assert access is only on the main thread
		commands_[key] = commandManager_.queueCommand(
			getClassIdentifier<SetReflectedPropertyRootObjectCommand>(), ObjectHandle(
			std::move(args), setter.getDefinitionManager()->getDefinition< ReflectedPropertyRootObjectCommandArgument >()));
	}

private:
	typedef std::pair< RefObjectId, std::string > Key;
	bool createKey( const PropertyAccessor & pa, Key & o_Key )
	{
		const auto & obj = pa.getRootObject();
		bool isOk = createKey( obj, *pa.getDefinitionManager(), o_Key );
		o_Key.second = pa.getFullPath();
		return isOk;
	}

	bool createKey(const ReflectedPropertyRootObjectSetter & setter, Key & o_Key)
	{
		const auto & obj = setter.getValue();
		bool isOk = createKey(obj, *setter.getDefinitionManager(), o_Key);
		return isOk;
	}

	bool createKey( const ObjectHandle& obj, const IDefinitionManager& defManager, Key & o_Key )
	{
		if (obj == nullptr)
		{
			return false;
		}

		if (!obj.getId(o_Key.first))
		{
			auto om = defManager.getObjectManager();
			assert(!om->getObject(obj.getStorage()->getRaw()).isValid());
			ObjectHandle oh = om->getUnmanagedObject(obj.getStorage()->getRaw());
			if (!oh.isValid())
			{
				o_Key.first = om->registerUnmanagedObject(const_cast<ObjectHandle&>(obj));
			}
			else
			{
				bool ok = oh.getId(o_Key.first);
				assert(ok);
			}
		}
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

ObjectHandle ReflectionController::getValue( const ReflectedPropertyRootObjectSetter & setter )
{
	return impl_->getValue( setter );
}

void ReflectionController::setValue( ReflectedPropertyRootObjectSetter & setter, const ObjectHandle & data )
{
	impl_->setValue( setter, data );
}