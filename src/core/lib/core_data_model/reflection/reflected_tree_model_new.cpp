#include "reflected_tree_model_new.hpp"
#include "reflected_tree_item_new.hpp"

#include <vector>


class ReflectedTreeModelNew::Implementation
{
public:
	Implementation( int columnCount );

	ReflectedTreeItemNew * getItemInternal( size_t index,
		const ReflectedTreeItemNew * parent ) const;
	size_t getIndexInternal( const ReflectedTreeItemNew * item ) const;
	size_t getChildCountInternal( const ReflectedTreeItemNew * item ) const;

	std::vector< ReflectedTreeItemNew * > rootItems_;
	int columnCount_;
};


ReflectedTreeModelNew::Implementation::Implementation( int columnCount )
	: columnCount_( columnCount )
{
}


ReflectedTreeItemNew * ReflectedTreeModelNew::Implementation::getItemInternal(
	size_t index,
	const ReflectedTreeItemNew * parent ) const
{
	if (parent == nullptr)
	{
		return index < rootItems_.size() ? rootItems_[ index ] : nullptr;
	}

	return parent->getChild( index );
}


size_t ReflectedTreeModelNew::Implementation::getIndexInternal(
	const ReflectedTreeItemNew * item ) const
{
	if (item == nullptr)
	{
		return 0;
	}

	auto parent = item->getParent();
	if (parent != nullptr)
	{
		const auto count = parent->size();
		for (size_t i = 0; i < count; ++i)
		{
			if (parent->getChild( i ) == item)
			{
				return i;
			}
		}
	}

	auto findIt = std::find( rootItems_.begin(), rootItems_.end(), item );
	assert( findIt != rootItems_.end() );
	return findIt - rootItems_.begin();
}


/**
 *	Get number of children in tree, at one level, under the given item.
 *	Including hidden items.
 *	E.g.
 *	> group1
 *	>> group2 <- hidden
 *	>>> item1 - count
 *	>>> item2 - count
 *	>>> item3 - count
 *	>> item4 - count
 *	getChildCountInternal( group1 ) == 2 // group2 and item4
 */
size_t ReflectedTreeModelNew::Implementation::getChildCountInternal(
	const ReflectedTreeItemNew * item ) const
{
	if (item == nullptr)
	{
		return rootItems_.size();
	}

	return item->size();
}


ReflectedTreeModelNew::ReflectedTreeModelNew( int columnCount )
	: AbstractTreeModel()
	, impl_( new Implementation( columnCount ) )
{
}


ReflectedTreeModelNew::~ReflectedTreeModelNew()
{
}


AbstractItem * ReflectedTreeModelNew::item(
	const AbstractTreeModel::ItemIndex & index ) const /* override */
{
	auto reflectedParent = dynamic_cast< const ReflectedTreeItemNew * >( index.parent_ );
	assert( index.parent_ == nullptr || reflectedParent != nullptr );

	auto itemCount = impl_->getChildCountInternal( reflectedParent );
	for (size_t i = 0; i < itemCount; ++i)
	{
		auto item = impl_->getItemInternal( i, reflectedParent );
		if (item != nullptr && item->hidden())
		{
			auto childItemCount = impl_->getChildCountInternal( item );
			if (index < childItemCount)
			{
				return this->item( index, item );
			}
			index -= childItemCount;
		}
		else
		{
			if (index == 0)
			{
				return item;
			}
			--index;
		}
	}
	return nullptr;
}


AbstractTreeModel::ItemIndex ReflectedTreeModelNew::index(
	const AbstractItem * item ) const /* override */
{
	if (item == nullptr)
	{
		return AbstractTreeModel::ItemIndex();
	}

	auto reflectedItem = dynamic_cast< const ReflectedTreeItemNew * >( item );
	assert( reflectedItem != nullptr );

	size_t index = 0;
	auto parent = reflectedItem->getParent();
	auto indexInternal = impl_->getIndexInternal( reflectedItem );
	for (size_t i = 0; i < indexInternal; ++i)
	{
		auto itemInternal = impl_->getItemInternal( i, parent );
		index += itemInternal != nullptr && itemInternal->hidden() ? 
			this->size( itemInternal ) : 1;
	}

	if (parent != nullptr && parent->hidden())
	{
		auto parentIndex = this->index( parent );
		index += parentIndex.first;
		parent = const_cast< ReflectedTreeItemNew *>(
			dynamic_cast< const ReflectedTreeItemNew * >( parentIndex.second ) );
	}
	return AbstractTreeModel::ItemIndex( index, parent );
}


int ReflectedTreeModelNew::rowCount(
	const AbstractItem * item ) const /* override */
{
	auto reflectedItem = static_cast< const ReflectedTreeItemNew * >( item );
	assert( item == nullptr || reflectedItem != nullptr );

	int count = 0;
	auto childCount = impl_->getChildCountInternal( reflectedItem );
	for (size_t i = 0; i < childCount; ++i)
	{
		auto childItem = impl_->getItemInternal( i, reflectedItem );
		count += childItem != nullptr && childItem->hidden() ? this->rowCount( childItem ) : 1;
	}
	return count;
}


int ReflectedTreeModelNew::columnCount() const /* override */
{
	return impl_->columnCount_;
}


void ReflectedTreeModelNew::addRootItem( AbstractItem * item )
{
	auto reflectedItem = dynamic_cast< ReflectedTreeModelNew * >( item );
	assert( reflectedItem != nullptr );
	assert( reflectedItem->getParent() == nullptr );
	assert( reflectedItem->model_ == nullptr );
	reflectedItem->model_ = this;

	//size_t index = impl_->rootItems_.size();
	//signalPreItemsInserted(nullptr, index, 1);
	impl_->rootItems_.push_back( reflectedItem );
	//signalPostItemsInserted(nullptr, index, 1);
}

void ReflectedTreeModelNew::removeRootItem( ReflectedTreeItemNew * item )
{
	auto reflectedItem = dynamic_cast< ReflectedTreeModelNew * >( item );
	assert( reflectedItem != nullptr );
	assert( reflectedItem != nullptr );
	assert( reflectedItem->model_ == this );
	const auto foundItr =
		std::find( impl_->rootItems_.cbegin(), impl_->rootItems_.cend(), reflectedItem );
	assert( foundItr != rootItems_.cend() );

	reflectedItem->model_ = nullptr;
	auto findIter = std::find( impl_->rootItems_.cbegin(),
		impl_->rootItems_.cend(),
		reflectedItem );
	if (findIter == impl_->rootItems_.end())
	{
		return;
	}

	//size_t index = std::distance( findIter, impl_->rootItems_.cbegin() );
	//signalPreItemsRemoved(nullptr, index, 1);
	impl_->rootItems_.erase( foundItr );
	//signalPostItemsRemoved(nullptr, index, 1);
}
