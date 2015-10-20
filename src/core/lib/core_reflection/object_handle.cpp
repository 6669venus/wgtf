#include "object_handle.hpp"
#include "reflected_object.hpp"
#include "i_definition_manager.hpp"
#include "i_object_manager.hpp"
#include "core_reflection/generic/generic_object.hpp"

//==============================================================================
// ObjectHandle
//==============================================================================

//------------------------------------------------------------------------------
/*static */ObjectHandle ObjectHandle::getHandle( ReflectedPolyStruct & value )
{
	auto defManager = value.getDefinition().getDefinitionManager();
	return defManager->getObjectManager()->getObject( &value );
}


//------------------------------------------------------------------------------
ObjectHandle::ObjectHandle()
	: storage_( nullptr )
{
}


//--------------------------------------------------------------------------
ObjectHandle::ObjectHandle( const ObjectHandle & other )
	: storage_( other.storage_ )
{
}


ObjectHandle::ObjectHandle( ObjectHandle && other )
	: storage_( std::move( other.storage_ ) )
{
}


//--------------------------------------------------------------------------
ObjectHandle::ObjectHandle( const std::shared_ptr< IObjectHandleStorage > & storage )
	: storage_( storage )
{
}


//------------------------------------------------------------------------------
ObjectHandle::ObjectHandle( const std::nullptr_t & )
	: storage_( nullptr )
{
}


//------------------------------------------------------------------------------
void * ObjectHandle::data() const
{
	return storage_ != nullptr ? storage_->data() : nullptr;
}


//------------------------------------------------------------------------------
TypeId ObjectHandle::type() const
{
	return storage_ != nullptr ? storage_->type() : nullptr;
}


//------------------------------------------------------------------------------
bool ObjectHandle::isValid() const
{
	return data() != nullptr;
}


//------------------------------------------------------------------------------
std::shared_ptr< IObjectHandleStorage > ObjectHandle::storage() const
{
	return storage_;
}


//------------------------------------------------------------------------------
const IClassDefinition * ObjectHandle::getDefinition( const IDefinitionManager & definitionManager ) const
{
	const IClassDefinition * definition = nullptr;

	auto type = this->type();

	// Check if the type is a generic type
	// Generic types will provide different definitions for each instance
	auto result = reflectedCast< const DefinitionProvider >(
		(*this),
		definitionManager );
	if (result != nullptr)
	{
		auto genericObject = result.get();
		assert( genericObject != nullptr );
		definition = &genericObject->getDefinition();
	}

	// Otherwise it's a static type
	// Static types will always provide the same type, so it can be looked up
	// with the class' name
	else
	{
		definition = ( type != nullptr ? definitionManager.getDefinition( type.getName() ) : nullptr );
	}

	auto storageDefinition = storage_ != nullptr ? storage_->getDefinition( definitionManager ) : nullptr;
	if ( storageDefinition != nullptr && definition != storageDefinition )
	{
		DEPRECATE_OBJECT_HANDLE_MSG( "DEPRECATED OBJECTHANDLE: Definition '%s' stored in ObjectHandle does not match inferred definition '%s'\n", storageDefinition->getName(), definition ? definition->getName() : "Unknown" );
	}

	return definition;
}


//------------------------------------------------------------------------------
bool ObjectHandle::getId( RefObjectId & o_Id ) const 
{
	if (storage_ == nullptr)
	{
		return false;
	}

	return storage_->getId( o_Id );
}


//------------------------------------------------------------------------------
void ObjectHandle::throwBase() const
{
	if (storage_ == nullptr)
	{
		return;
	}

	storage_->throwBase();
}


//------------------------------------------------------------------------------
bool ObjectHandle::operator ==( const ObjectHandle & other ) const
{
	if (storage_ == other.storage_)
	{
		return true;
	}

	if (storage_ == nullptr || other.storage_ == nullptr)
	{
		return false;
	}

	auto left = storage_->data();
	auto right = other.storage_->data();
	if (left == right)
	{
		if (storage_->type() == other.storage_->type())
		{
			return true;
		}
	}
	return false;
}


//------------------------------------------------------------------------------
bool ObjectHandle::operator !=( const ObjectHandle & other ) const
{
	return !operator==( other );
}


//------------------------------------------------------------------------------
ObjectHandle & ObjectHandle::operator=( const std::nullptr_t & )
{
	storage_ = nullptr;
	return *this;
}


//------------------------------------------------------------------------------
ObjectHandle & ObjectHandle::operator=( const ObjectHandle & other )
{
	storage_ = other.storage_;
	return *this;
}


//------------------------------------------------------------------------------
ObjectHandle & ObjectHandle::operator=( ObjectHandle && other )
{
	storage_ = std::move( other.storage_ );
	return *this;
}


//------------------------------------------------------------------------------
ObjectHandle & ObjectHandle::operator=( const std::shared_ptr< IObjectHandleStorage > & storage )
{
	storage_ = storage;
	return *this;
}


//------------------------------------------------------------------------------
bool ObjectHandle::operator<( const ObjectHandle & other ) const
{
	if (storage_ == other.storage_)
	{
		return false;
	}

	if (storage_ == nullptr)
	{
		return true;
	}

	if (other.storage_ == nullptr)
	{
		return false;
	}

	auto left = storage_->data();
	auto right = other.storage_->data();
	if (left == right)
	{
		return storage_->type() < other.storage_->type();
	}
	return left < right;
}


//------------------------------------------------------------------------------
void * reflectedCast( void * source, const TypeId & typeIdSource, const TypeId & typeIdDest, const IDefinitionManager & definitionManager )
{
	char * pRaw = static_cast< char * >( source );
	if (pRaw == nullptr)
	{
		return pRaw;
	}

	if (typeIdSource == typeIdDest)
	{
		return pRaw;
	}

	auto srcDefinition = definitionManager.getDefinition( typeIdSource.getName() );
	if (srcDefinition != nullptr)
	{
		auto helperCache = srcDefinition->getDetails().getCastHelperCache();
		auto helperFindIt = helperCache->find( typeIdDest );
		if (helperFindIt != helperCache->end())
		{
			if (!helperFindIt->second.first)
			{
				return nullptr;
			}
			return pRaw + helperFindIt->second.second;
		}

		auto dstDefinition = definitionManager.getDefinition( typeIdDest.getName() );
		if (dstDefinition != nullptr &&
			srcDefinition->canBeCastTo( *dstDefinition ))
		{
			auto result = srcDefinition->castTo( *dstDefinition, pRaw);
			assert( result != nullptr );
			helperCache->insert(
				std::make_pair(
				typeIdDest,
				std::make_pair( true,
				reinterpret_cast< const char * >( result ) - pRaw ) ) );
			return result;
		}
		else
		{
			helperCache->insert(
				std::make_pair(
				typeIdDest,
				std::make_pair( false,
				0 ) ) );
			return nullptr;
		}
	}

	return nullptr;
}


//------------------------------------------------------------------------------
ObjectHandle reflectedRoot( const ObjectHandle & source, const IDefinitionManager & defintionManager )
{
	if (!source.isValid())
	{
		return source;
	}

	auto root = source.storage();
	auto reflectedRoot = 
		root->type() == TypeId::getType< GenericObject >() || 
		defintionManager.getDefinition( root->type().getName() ) != nullptr ? root : nullptr;
	for (;;)
	{
		auto inner = root->inner();
		if (inner == nullptr)
		{
			break;
		}
		root = inner;
		reflectedRoot = 
			root->type() == TypeId::getType< GenericObject >() || 
			defintionManager.getDefinition( root->type().getName() ) != nullptr ? root : reflectedRoot;
	}
	return ObjectHandle( reflectedRoot );
}