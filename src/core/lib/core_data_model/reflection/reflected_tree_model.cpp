#include "reflected_tree_model.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/reflection/reflected_property_item.hpp"
#include "core_reflection/property_accessor_listener.hpp"

class ReflectedTreeModelPropertyListener
	: public PropertyAccessorListener
{
public:
	ReflectedTreeModelPropertyListener( ReflectedItem & item )
		: rootItem_( item )
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
	ReflectedItem & rootItem_;
};


//==============================================================================
ReflectedTreeModel::ReflectedTreeModel(
	const ObjectHandle & object,
	IDefinitionManager & definitionManager,
	IReflectionController * controller )
	: base( 2 )
	, rootItem_( new ReflectedObjectItem( object, nullptr /* parent */ ) )
	, definitionManager_( definitionManager )
	, listener_( new ReflectedTreeModelPropertyListener( *rootItem_.get() ) )
{
	definitionManager_.registerPropertyAccessorListener( listener_ );

	rootItem_->setController( controller );
	rootItem_->setDefinitionManager( &definitionManager_ );
	this->addRootItem( rootItem_.get() );
}


//==============================================================================
ReflectedTreeModel::~ReflectedTreeModel()
{
	this->removeRootItem( rootItem_.get() );
	definitionManager_.deregisterPropertyAccessorListener( listener_ );
}


void ReflectedTreeModel::addRootItem( GenericTreeItem * item ) /* override */
{
	// ReflectedTreeModel does not support multiple roots
	assert( item == rootItem_.get() );
	base::addRootItem( item );
}


void ReflectedTreeModel::removeRootItem( GenericTreeItem * item ) /* override */
{
	// ReflectedTreeModel does not support multiple roots
	assert( item == rootItem_.get() );
	base::removeRootItem( item );
}

//==============================================================================
void ReflectedTreeModelPropertyListener::preSetValue( 
	const PropertyAccessor & accessor, const Variant & value )
{
	rootItem_.preSetValue( accessor, value );
}


//==============================================================================
void ReflectedTreeModelPropertyListener::postSetValue( 
	const PropertyAccessor & accessor, const Variant & value )
{
	rootItem_.postSetValue( accessor, value );
}

void ReflectedTreeModelPropertyListener::preItemsInserted( const PropertyAccessor & accessor, 
	const Collection::ConstIterator & pos, size_t count )
{
	rootItem_.preItemsInserted( accessor, pos, count );
}

void ReflectedTreeModelPropertyListener::postItemsInserted( const PropertyAccessor & accessor, 
	const Collection::ConstIterator & begin, const Collection::ConstIterator & end )
{
	rootItem_.postItemsInserted( accessor, begin, end );
}

void ReflectedTreeModelPropertyListener::preItemsRemoved( const PropertyAccessor & accessor,
	const Collection::ConstIterator & begin, const Collection::ConstIterator & end )
{
	rootItem_.preItemsRemoved( accessor, begin, end );
}

void ReflectedTreeModelPropertyListener::postItemsRemoved( const PropertyAccessor & accessor,
	const Collection::ConstIterator & pos, size_t count )
{
	rootItem_.postItemsRemoved( accessor, pos, count );
}