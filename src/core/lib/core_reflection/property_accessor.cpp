#include "property_accessor.hpp"
#include "interfaces/i_base_property.hpp"
#include "reflected_object.hpp"
#include "object_handle.hpp"
#include "property_accessor_listener.hpp"
#include "i_definition_manager.hpp"
#include "utilities/reflection_utilities.hpp"

#include <unordered_map>
#include "core_variant/variant.hpp"

//==============================================================================
PropertyAccessor::PropertyAccessor( PropertyAccessor && other )
	: rootObject_( other.rootObject_ )
	, path_( std::move( other.path_ ) )
	, object_( other.object_ )
	, property_( other.property_ )
	, sharedProperty_( std::move( other.sharedProperty_ ) )
	, definitionManager_( other.definitionManager_ )
{
}


//==============================================================================
PropertyAccessor::PropertyAccessor(const PropertyAccessor & other)
	: rootObject_(other.rootObject_)
	, path_( other.path_ )
	, object_(other.object_)
	, property_(other.property_)
	, sharedProperty_( other.sharedProperty_ )
	, definitionManager_(other.definitionManager_)
{
}


//==============================================================================
PropertyAccessor::PropertyAccessor()
	: property_( nullptr )
	, definitionManager_( nullptr )
{
}


//==============================================================================
PropertyAccessor::PropertyAccessor(
	const IDefinitionManager * definitionManager,
	const ObjectHandle & baseProvider, const char * path )
	: rootObject_( baseProvider )
	, path_( path )
	, property_( nullptr )
	, definitionManager_( definitionManager )
{
}


//==============================================================================
bool PropertyAccessor::isValid() const
{
	return object_ != ObjectHandle() && getProperty() != NULL;
}


//==============================================================================
const TypeId & PropertyAccessor::getType() const
{
	if (!isValid())
	{
		static auto s_voidType = TypeId::getType< void >();
		return s_voidType;
	}
	return getProperty()->getType();
}


//==============================================================================
PropertyAccessor PropertyAccessor::getParent() const
{
	if (path_.length() == 0||
		definitionManager_ == nullptr)
	{
		return PropertyAccessor();
	}
	auto definition = rootObject_.getDefinition();
	if (definition == nullptr)
	{
		return PropertyAccessor();
	}
	// This is the member operator '.', the arrow operator '->', and array operator '['
	size_t parentMarkerPos = path_.find_last_of( ".[" );
	if (parentMarkerPos == std::string::npos)
	{
		return definition->bindProperty( "", rootObject_ );
	}
	std::string parentPath;
	parentPath.append( path_.c_str(), parentMarkerPos );
	return definition->bindProperty( parentPath.c_str(), rootObject_ );
}


//==============================================================================
bool PropertyAccessor::setValue( const Variant & value ) const 
{
	if (!isValid())
	{
		return false;
	}
	if (definitionManager_ == nullptr)
	{
		return false;
	}
	// Since "listeners" is a MutableVector, these iterators are safe to use
	// while other listeners are registered/deregistered
	auto& listeners = definitionManager_->getPropertyAccessorListeners();
	auto itBegin = listeners.cbegin();
	auto itEnd = listeners.end();
	for( auto it = itBegin; it != itEnd; ++it )
	{
		(*it).get()->preSetValue( *this, value );
	}
	bool ret = getProperty()->set( object_, value );
	for( auto it = itBegin; it != itEnd; ++it )
	{
		(*it).get()->postSetValue( *this, value );
	}
	return ret;
}


//==============================================================================
bool PropertyAccessor::setValueWithoutNotification( const Variant & value ) const 
{
	if (!isValid())
	{
		return false;
	}
	if (definitionManager_ == nullptr)
	{
		return false;
	}

	return getProperty()->set( object_, value );
}


//==============================================================================
const char * PropertyAccessor::getName() const
{
	if(!isValid())
	{
		return NULL;
	}
	return getProperty()->getName();
}


//==============================================================================
const IClassDefinition * PropertyAccessor::getStructDefinition() const
{
	if(!isValid())
	{
		return nullptr;
	}
	if (definitionManager_ == nullptr)
	{
		return nullptr;
	}
	Collection collection;
	bool isCollection = getValue().tryCast( collection );
	if (isCollection)
	{
		return 
			definitionManager_->getDefinition(
			collection.valueType().getName()
		);
	}
	return definitionManager_->getDefinition(
		getProperty()->getType().getName() );
}


//==============================================================================
Variant PropertyAccessor::getValue() const
{
	if(!isValid())
	{
		return getRootObject();
	}

	return getProperty()->get( object_ );
}


//==============================================================================
const MetaBase * PropertyAccessor::getMetaData() const
{
	if (getProperty() == NULL)
	{
		return NULL;
	}
	return getProperty()->getMetaData();
}


//==============================================================================
void PropertyAccessor::setObject( const ObjectHandle & object )
{
	object_ = object;
}


//==============================================================================
void PropertyAccessor::setBaseProperty( IBaseProperty * property )
{
	sharedProperty_.reset();
	property_ = property;
}


//==============================================================================
void PropertyAccessor::setBaseProperty(
	const std::shared_ptr< IBaseProperty > & property )
{
	sharedProperty_ = property;
	property_ = property.get();
}


//==============================================================================
const ObjectHandle & PropertyAccessor::getRootObject() const
{
	return rootObject_;
}


//==============================================================================
const char * PropertyAccessor::getFullPath() const
{
	return path_.c_str();
}


//==============================================================================
void PropertyAccessor::firePreItemsInserted( const Collection::ConstIterator & pos, 
										   size_t count ) const
{
	if (definitionManager_ == nullptr)
	{
		return;
	}
	// Since "listeners" is a MutableVector, these iterators are safe to use
	// while other listeners are registered/deregistered
	auto& listeners = definitionManager_->getPropertyAccessorListeners();
	auto itBegin = listeners.cbegin();
	auto itEnd = listeners.end();
	for( auto it = itBegin; it != itEnd; ++it )
	{
		(*it).get()->preItemsInserted( *this, pos, count );
	}
}


//==============================================================================
void PropertyAccessor::firePostItemsInserted( const Collection::ConstIterator & begin,
											const Collection::ConstIterator & end ) const
{
	if (definitionManager_ == nullptr)
	{
		return;
	}
	// Since "listeners" is a MutableVector, these iterators are safe to use
	// while other listeners are registered/deregistered
	auto& listeners = definitionManager_->getPropertyAccessorListeners();
	auto itBegin = listeners.cbegin();
	auto itEnd = listeners.end();
	for( auto it = itBegin; it != itEnd; ++it )
	{
		(*it).get()->postItemsInserted( *this, begin, end );
	}
}


//==============================================================================
void PropertyAccessor::firePreItemsRemoved( const Collection::ConstIterator & begin,
										   const Collection::ConstIterator & end ) const
{
	if (definitionManager_ == nullptr)
	{
		return;
	}
	// Since "listeners" is a MutableVector, these iterators are safe to use
	// while other listeners are registered/deregistered
	auto& listeners = definitionManager_->getPropertyAccessorListeners();
	auto itBegin = listeners.cbegin();
	auto itEnd = listeners.end();
	for( auto it = itBegin; it != itEnd; ++it )
	{
		(*it).get()->preItemsRemoved( *this, begin, end );
	}
}


//==============================================================================
void PropertyAccessor::firePostItemsRemoved( const Collection::ConstIterator & pos, 
											size_t count ) const
{
	if (definitionManager_ == nullptr)
	{
		return;
	}
	// Since "listeners" is a MutableVector, these iterators are safe to use
	// while other listeners are registered/deregistered
	auto& listeners = definitionManager_->getPropertyAccessorListeners();
	auto itBegin = listeners.cbegin();
	auto itEnd = listeners.end();
	for( auto it = itBegin; it != itEnd; ++it )
	{
		(*it).get()->postItemsRemoved( *this, pos, count );
	}
}


//==============================================================================
const IDefinitionManager * PropertyAccessor::getDefinitionManager() const
{
	return definitionManager_;
}


//==============================================================================
PropertyAccessor & PropertyAccessor::operator =( const PropertyAccessor && other )
{
	rootObject_ = other.rootObject_;
	path_ = other.path_;
	object_ = other.object_;
	property_ = other.property_;
	sharedProperty_ = other.sharedProperty_;
	definitionManager_ = other.definitionManager_;
	return *this;
}