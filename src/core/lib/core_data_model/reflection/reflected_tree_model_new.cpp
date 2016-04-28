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
	int getChildCountInternal( const ReflectedTreeItemNew * item ) const;

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
		const auto count = parent->rowCount();
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
int ReflectedTreeModelNew::Implementation::getChildCountInternal(
	const ReflectedTreeItemNew * item ) const
{
	if (item == nullptr)
	{
		return static_cast< int >( rootItems_.size() );
	}

	return item->rowCount();
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
	auto row = index.row_;
	for (int i = 0; i < itemCount; ++i)
	{
		auto item = impl_->getItemInternal( i, reflectedParent );
		if (item != nullptr && !item->isCollection())
		{
			auto childItemCount = impl_->getChildCountInternal( item );
			if (row < childItemCount)
			{
				const AbstractTreeModel::ItemIndex parentIndex( row, item );
				return this->item( parentIndex );
			}
			row -= childItemCount;
		}
		else
		{
			if (row == 0)
			{
				return item;
			}
			--row;
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

	int row = 0;
	auto parent = reflectedItem->getParent();
	auto indexInternal = impl_->getIndexInternal( reflectedItem );
	for (size_t i = 0; i < indexInternal; ++i)
	{
		auto itemInternal = impl_->getItemInternal( i, parent );
		row += itemInternal != nullptr && !itemInternal->isCollection() ? 
			this->rowCount( itemInternal ) : 1;
	}

	if (parent != nullptr && !parent->isCollection())
	{
		auto parentIndex = this->index( parent );
		row += parentIndex.row_;
		parent = const_cast< ReflectedTreeItemNew *>(
			dynamic_cast< const ReflectedTreeItemNew * >( parentIndex.parent_ ) );
		assert( parentIndex.parent_ == nullptr || parent != nullptr );
	}
	return AbstractTreeModel::ItemIndex( row, parent );
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
		count += childItem != nullptr && !childItem->isCollection() ? this->rowCount( childItem ) : 1;
	}
	return count;
}


int ReflectedTreeModelNew::columnCount() const /* override */
{
	return impl_->columnCount_;
}


void ReflectedTreeModelNew::addRootItem( AbstractItem * item )
{
	auto reflectedItem = dynamic_cast< ReflectedTreeItemNew * >( item );
	assert( reflectedItem != nullptr );
	assert( reflectedItem->getParent() == nullptr );
	assert( reflectedItem->getModel() == nullptr );
	reflectedItem->setModel( this );

	const int row = static_cast< int >( impl_->rootItems_.size() );
	const AbstractItem * pParent = nullptr;
	const ItemIndex index( row, pParent );
	const int pos = 0; // TODO what is pos??
	const int rowCount = reflectedItem != nullptr ? reflectedItem->rowCount() : 0;

	preRowsInserted_( index, pos, rowCount );
	impl_->rootItems_.emplace_back( reflectedItem );
	postRowsInserted_( index, pos, rowCount );
}


void ReflectedTreeModelNew::removeRootItem( AbstractItem * item )
{
	auto reflectedItem = dynamic_cast< ReflectedTreeItemNew * >( item );
	assert( reflectedItem != nullptr );
	assert( reflectedItem != nullptr );
	assert( reflectedItem->getModel() == this );
	const auto foundItr =
		std::find( impl_->rootItems_.cbegin(), impl_->rootItems_.cend(), reflectedItem );
	assert( foundItr != impl_->rootItems_.cend() );

	reflectedItem->setModel( nullptr );
	auto foundIter = std::find( impl_->rootItems_.cbegin(),
		impl_->rootItems_.cend(),
		reflectedItem );
	if (foundIter == impl_->rootItems_.end())
	{
		return;
	}
	const auto pItem = (*foundIter);

	const int row = static_cast< int >(
		std::distance( foundIter, impl_->rootItems_.cbegin() ) );
	const AbstractItem * pParent = nullptr;
	const ItemIndex index( row, pParent );
	const int pos = 0; // TODO what is pos??
	const int rowCount = pItem != nullptr ? pItem->rowCount() : 0;

	preRowsRemoved_( index, pos, rowCount );
	impl_->rootItems_.erase( foundItr );
	postRowsRemoved_( index, pos, rowCount );
}


#define CONNECT_METHOD( method, connection, callbackType ) \
Connection ReflectedTreeModelNew::method( \
	AbstractTreeModel::callbackType callback ) /* override */ \
{ \
	return connection.connect( callback ); \
} \

CONNECT_METHOD( connectPreItemDataChanged, preItemDataChanged_, DataCallback )
CONNECT_METHOD( connectPostItemDataChanged, postItemDataChanged_, DataCallback )

CONNECT_METHOD( connectPreRowsInserted, preRowsInserted_, RangeCallback )
CONNECT_METHOD( connectPostRowsInserted, postRowsInserted_, RangeCallback )

CONNECT_METHOD( connectPreRowsRemoved, preRowsRemoved_, RangeCallback )
CONNECT_METHOD( connectPostRowsRemoved, postRowsRemoved_, RangeCallback )

#undef CONNECT_METHOD
