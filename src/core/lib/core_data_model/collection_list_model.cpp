#include "collection_list_item.hpp"
#include "collection_list_model.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include <cassert>


class CollectionListModelPropertyListener
	: public PropertyAccessorListener
{
public:
	CollectionListModelPropertyListener( CollectionListModel::Impl & impl )
		: impl_( impl )
	{
	}

	// PropertyAccessorListener
	void preSetValue(
		const PropertyAccessor & accessor, const Variant & value ) override;
	void postSetValue(
		const PropertyAccessor & accessor, const Variant & value ) override;
	void preItemsInserted( const PropertyAccessor & accessor, 
		const Collection::ConstIterator & pos, size_t count ) override;
	void postItemsInserted( const PropertyAccessor & accessor, 
		const Collection::ConstIterator & begin, const Collection::ConstIterator & end ) override;
	void preItemsRemoved( const PropertyAccessor & accessor,
		const Collection::ConstIterator & begin, const Collection::ConstIterator & end ) override;
	void postItemsRemoved( const PropertyAccessor & accessor,
		const Collection::ConstIterator & pos, size_t count ) override;
private:
	CollectionListModel::Impl & impl_;
};

class CollectionListModel::Impl
{
public:
	Impl( const ObjectHandle & object,
		const char* path, CollectionListModel * listModel );
	~Impl();
	ObjectHandle object_;
	std::string path_;
	CollectionListModel * getCollectionListModel();
private:
	CollectionListModel * listModel_;
	std::shared_ptr< PropertyAccessorListener > listener_;
};


CollectionListModel::Impl::Impl( const ObjectHandle & object,
	const char* path, CollectionListModel * listModel )
	: object_( object )
	, path_( path )
	, listModel_( listModel )
	, listener_( new CollectionListModelPropertyListener( *this ) )
{
	IDefinitionManager* pDefinitionManager =
		object_.getDefinition()->getDefinitionManager();
	assert( pDefinitionManager != nullptr );
	pDefinitionManager->registerPropertyAccessorListener( listener_ );
}

CollectionListModel::Impl::~Impl()
{
	IDefinitionManager* pDefinitionManager =
		object_.getDefinition()->getDefinitionManager();
	assert( pDefinitionManager != nullptr );
	pDefinitionManager->deregisterPropertyAccessorListener( listener_ );
}

CollectionListModel * CollectionListModel::Impl::getCollectionListModel()
{
	return listModel_;
}

CollectionListModel::CollectionListModel( const ObjectHandle & object,
	const char* path )
	: IListModel()
	, impl_( new Impl( object, path, this ) )
{
}


CollectionListModel::~CollectionListModel()
{
	
}


size_t CollectionListModel::index( const IItem* item ) const
{
	assert( false && "Not implemented" );
	return 0;
}


bool CollectionListModel::empty() const
{
	auto propertyAccessor = impl_->object_.getDefinition()->bindProperty( 
		impl_->path_.c_str(), impl_->object_ );
	assert( propertyAccessor.isValid() );
	Collection collection;
	const bool success = propertyAccessor.getValue().tryCast( collection );
	assert( success );
	return collection.empty();
}


size_t CollectionListModel::size() const
{
	auto propertyAccessor = impl_->object_.getDefinition()->bindProperty( 
		impl_->path_.c_str(), impl_->object_ );
	assert( propertyAccessor.isValid() );
	Collection collection;
	const bool success = propertyAccessor.getValue().tryCast( collection );
	assert( success );
	return collection.size();
}


void CollectionListModelPropertyListener::preSetValue( const PropertyAccessor& accessor,
	const Variant& value )
{
	if (impl_.path_ != accessor.getFullPath())
	{
		return;
	}

	assert( false && "Not implemented" );
}


void CollectionListModelPropertyListener::postSetValue( const PropertyAccessor& accessor,
	const Variant& value )
{
	if (impl_.path_ != accessor.getFullPath())
	{
		return;
	}

	assert( false && "Not implemented" );
}


void CollectionListModelPropertyListener::preItemsInserted( const PropertyAccessor& accessor, 
	const Collection::ConstIterator& pos,
	size_t count )
{
	if (impl_.path_ != accessor.getFullPath())
	{
		return;
	}
	assert( count >= 1 );

	auto propertyAccessor = impl_.object_.getDefinition()->bindProperty( 
		impl_.path_.c_str(), impl_.object_ );
	assert( propertyAccessor.isValid() );
	Collection collection;
	const bool success = propertyAccessor.getValue().tryCast( collection );
	assert( success );

	const IItem* parent = nullptr;
	const size_t startIndex = std::distance( collection.cbegin(), pos );
	impl_.getCollectionListModel()->notifyPreItemsInserted( parent, startIndex, count );
}


void CollectionListModelPropertyListener::postItemsInserted( const PropertyAccessor& accessor,
	const Collection::ConstIterator& begin,
	const Collection::ConstIterator& end )
{
	if (impl_.path_ != accessor.getFullPath())
	{
		return;
	}

	auto propertyAccessor = impl_.object_.getDefinition()->bindProperty( 
		impl_.path_.c_str(), impl_.object_ );
	assert( propertyAccessor.isValid() );
	Collection collection;
	const bool success = propertyAccessor.getValue().tryCast( collection );
	assert( success );

	const size_t startIndex = std::distance( collection.cbegin(), begin );
	const size_t count = std::distance( begin, end );
	assert( count >= 1 );

	const IItem* parent = nullptr;
	impl_.getCollectionListModel()->notifyPostItemsInserted( parent, startIndex, count );
}


void CollectionListModelPropertyListener::preItemsRemoved( const PropertyAccessor& accessor,
	const Collection::ConstIterator& begin,
	const Collection::ConstIterator& end )
{
	if (impl_.path_ != accessor.getFullPath())
	{
		return;
	}

	auto propertyAccessor = impl_.object_.getDefinition()->bindProperty( 
		impl_.path_.c_str(), impl_.object_ );
	assert( propertyAccessor.isValid() );
	Collection collection;
	const bool success = propertyAccessor.getValue().tryCast( collection );
	assert( success );

	const size_t startIndex = std::distance( collection.cbegin(), begin );
	const size_t count = std::distance( begin, end );
	assert( count >= 1 );
	const IItem* parent = nullptr;
	impl_.getCollectionListModel()->notifyPreItemsRemoved( parent, startIndex, count );
}


void CollectionListModelPropertyListener::postItemsRemoved( const PropertyAccessor& accessor,
	const Collection::ConstIterator& pos,
	size_t count )
{
	if (impl_.path_ != accessor.getFullPath())
	{
		return;
	}

	auto propertyAccessor = impl_.object_.getDefinition()->bindProperty( 
		impl_.path_.c_str(), impl_.object_ );
	assert( propertyAccessor.isValid() );
	Collection collection;
	const bool success = propertyAccessor.getValue().tryCast( collection );
	assert( success );

	const size_t startIndex = std::distance( collection.cbegin(), pos );
	assert( count >= 1 );
	const IItem* parent = nullptr;

	// TODO erase items
	assert( false && "Not implemented" );

	impl_.getCollectionListModel()->notifyPostItemsRemoved( parent, startIndex, count );
}
