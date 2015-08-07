#include "object_handle.hpp"
#include "reflected_object.hpp"
#include "i_definition_manager.hpp"
#include "i_object_manager.hpp"

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
ObjectHandle::ObjectHandle(
	const std::shared_ptr< IObjectHandleStorage > & storage )
	: storage_( storage )
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


//------------------------------------------------------------------------------
ObjectHandle::ObjectHandle( const nullptr_t & )
	: storage_( nullptr )
{
}


//------------------------------------------------------------------------------
void ObjectHandle::throwBase() const
{
	if(storage_ == nullptr)
	{
		return;
	}
	storage_->throwBase();
}


//------------------------------------------------------------------------------
bool ObjectHandle::getId( RefObjectId & o_Id ) const 
{
	return storage_->getId( o_Id );
}


//------------------------------------------------------------------------------
const std::shared_ptr< IObjectHandleStorage > & ObjectHandle::getStorage() const
{
	return storage_;
}


//------------------------------------------------------------------------------
const IClassDefinition * ObjectHandle::getDefinition() const
{
	return storage_ ? storage_->getDefinition() : nullptr;
}


//------------------------------------------------------------------------------
bool ObjectHandle::isValid() const
{
	return storage_ != nullptr && storage_->isValid();
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

	auto left = storage_->getRaw();
	auto right = other.storage_->getRaw();
	if (left == right)
	{
		if (storage_->getPointedType().getHashcode() ==
				other.storage_->getPointedType().getHashcode())
		{
			return true;
		}

		if (other.storage_->tryCast( storage_->getPointedType() ) ||
				storage_->tryCast( other.storage_->getPointedType() ))
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
ObjectHandle & ObjectHandle::operator=( const nullptr_t & )
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


ObjectHandle & ObjectHandle::operator=( ObjectHandle && other )
{
	storage_ = std::move( other.storage_ );
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

	auto left = storage_->getRaw();
	auto right = other.storage_->getRaw();
	if (left == right)
	{
		return
			storage_->getPointedType().getHashcode() <
			other.storage_->getPointedType().getHashcode();
	}
	return left < right;
}


//------------------------------------------------------------------------------