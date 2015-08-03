#include "reflected_tree_model.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/reflection/reflected_property_item.hpp"
#include "core_reflection/property_accessor_listener.hpp"

class ReflectedTreeModelPropertyListener
	: public PropertyAccessorListener
{
public:
	ReflectedTreeModelPropertyListener( ReflectedObjectItem & item )
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
	ReflectedObjectItem & rootItem_;
};


//==============================================================================
ReflectedTreeModel::ReflectedTreeModel(
	const ObjectHandle & object,
	IReflectionController * controller )
	: rootItem_( object )
	, listener_( new ReflectedTreeModelPropertyListener( rootItem_ ) )
{
	auto defManager = object.getDefinition()->getDefinitionManager();
	defManager->registerPropertyAccessorListener( listener_ );

	rootItem_.setController( controller );
	addRootItem( &rootItem_ );
}


//==============================================================================
ReflectedTreeModel::~ReflectedTreeModel()
{
	this->removeRootItem( &rootItem_ );
	auto obj = rootItem_.getObject();
	auto definition = obj.getDefinition();
	auto defManager = definition ? definition->getDefinitionManager() : nullptr;
	if (defManager)
	{
		defManager->deregisterPropertyAccessorListener( listener_ );
	}
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