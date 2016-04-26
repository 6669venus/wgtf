#include "reflected_tree_model_new.hpp"

#include <vector>


// Based on ReflectedItem/GenericTreeItem
class ReflectedTreeItem : public AbstractTreeItem
{
public:
	//virtual ReflectedTreeItem * getParent() const = 0;
	//virtual ReflectedTreeItem * getChild( size_t index ) const = 0;

	//virtual bool empty() const;
	//virtual size_t size() const = 0;
};


class ReflectedTreeModelNew::Implementation
{
public:
	std::vector< ReflectedTreeItem * > rootItems_;
};


ReflectedTreeModelNew::ReflectedTreeModelNew()
	: AbstractTreeModel()
	, impl_( new Implementation() )
{
}


ReflectedTreeModelNew::~ReflectedTreeModelNew()
{
}


AbstractItem * ReflectedTreeModelNew::item(
	const AbstractTreeModel::ItemIndex & index ) const /* override */
{
	const auto & rootItems = impl_->rootItems_;

	// Root item
	if (index.parent_ == nullptr)
	{
		if ((index.row_ < 0) || (index.row_ >= rootItems.size()))
		{
			return nullptr;
		}

		return rootItems.at( index.row_ );
	}

	// Child item
	// TODO AbstractTreeItem does not support parenting??

	return nullptr;
}


AbstractTreeModel::ItemIndex ReflectedTreeModelNew::index(
	const AbstractItem * item ) const /* override */
{
	const auto & rootItems = impl_->rootItems_;

	const auto found = std::find( rootItems.cbegin(), rootItems.cend(), item );
	if (found == rootItems.end())
	{
		return ItemIndex();
	}

	const auto row = std::distance( rootItems.cbegin(), found );
	return ItemIndex( static_cast< int >( row ) );
}


int ReflectedTreeModelNew::rowCount(
	const AbstractItem * item ) const /* override */
{
	return static_cast< int >( impl_->rootItems_.size() );
}


int ReflectedTreeModelNew::columnCount() const /* override */
{
	// TODO Seems to be no way to check if AbstractItems all have the same column count??
	return 2;
}
