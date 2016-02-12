#include "reflected_list.hpp"

#include "core_reflection/property_accessor.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include "core_reflection/definition_manager.hpp"

#include "core_data_model/i_item_role.hpp"

class ReflectedListListener
	: public PropertyAccessorListener
{
public:
	ReflectedListListener( const ReflectedList & list )
		: list_( list )
	{
	}

	// PropertyAccessorListener
	void preSetValue( const PropertyAccessor & accessor,
		const Variant & value ) override;
	void postSetValue( const PropertyAccessor & accessor,
		const Variant & value ) override;
	void preItemsInserted( const PropertyAccessor & accessor, 
		const Collection::ConstIterator & pos, size_t count ) override;
	void postItemsInserted( const PropertyAccessor & accessor, 
		const Collection::ConstIterator & begin, const Collection::ConstIterator & end ) override;
	void preItemsRemoved( const PropertyAccessor & accessor,
		const Collection::ConstIterator & begin, const Collection::ConstIterator & end ) override;
	void postItemsRemoved( const PropertyAccessor & accessor,
		const Collection::ConstIterator & pos, size_t count ) override;

private:
	const IItem* find(const PropertyAccessor & accessor);
	size_t findIndex(const Variant obj);

	const ReflectedList & list_;
};

ReflectedList::ReflectedList( IDefinitionManager* defManager )
	: listener_( new ReflectedListListener( *this ) )
	, defManager_( defManager )
{
	defManager_->registerPropertyAccessorListener( listener_ );
}

ReflectedList::~ReflectedList()
{
	defManager_->deregisterPropertyAccessorListener( listener_ );
}

const IItem* ReflectedListListener::find(const PropertyAccessor & accessor)
{
	const Variant obj = accessor.getRootObject();
	auto it = std::find_if( list_.cbegin(), list_.cend(),
		[&](const Variant& item) { return obj == item; } );
	return (it != list_.cend()) ? list_.item( it - list_.cbegin() ) : nullptr;
}

size_t ReflectedListListener::findIndex(const Variant obj)
{
	auto it = std::find_if( list_.cbegin(), list_.cend(),
		[&](const Variant& item) { return obj == item; } );
	return (it != list_.cend()) ? it - list_.cbegin() : -1;
}

void ReflectedListListener::preSetValue( const PropertyAccessor & accessor, const Variant & value )
{
	if (auto item = find(accessor))
	{
		list_.onPreDataChanged( item, 0, DefinitionRole::roleId_, value );
	}
}

void ReflectedListListener::postSetValue( 
	const PropertyAccessor & accessor, const Variant & value )
{
	if (auto item = find(accessor))
	{
		list_.onPostDataChanged( item, 0, DefinitionRole::roleId_, value );
	}
}

void ReflectedListListener::preItemsInserted( 
	const PropertyAccessor & accessor, const Collection::ConstIterator & pos, size_t count )
{
	size_t index = findIndex(*pos);
	assert(index < list_.size());
	list_.onPreItemsInserted( index, count );
}

void ReflectedListListener::postItemsInserted(
	const PropertyAccessor & accessor,
	const Collection::ConstIterator & begin, 
	const Collection::ConstIterator & end )
{
	size_t ib = findIndex(*begin);
	size_t ie = findIndex(*end);
	assert(ib <= ie);
	assert(ie < list_.size());
	list_.onPreItemsInserted( ib, ie - ib );
}

void ReflectedListListener::preItemsRemoved( 
	const PropertyAccessor & accessor,
	const Collection::ConstIterator & begin,
	const Collection::ConstIterator & end )
{
	size_t ib = findIndex(*begin);
	size_t ie = findIndex(*end);
	assert(ib <= ie);
	assert(ie < list_.size());
	list_.onPreItemsRemoved( ib, ie - ib );
}

void ReflectedListListener::postItemsRemoved( 
	const PropertyAccessor & accessor,
	const Collection::ConstIterator & pos,
	size_t count )
{
	size_t index = findIndex(*pos);
	assert(index < list_.size());
	list_.onPostItemsRemoved( index, count );
}
