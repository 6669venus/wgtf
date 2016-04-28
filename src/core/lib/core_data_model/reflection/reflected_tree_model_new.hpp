#pragma once
#ifndef _REFLECTED_TREE_MODEL_NEW_HPP
#define _REFLECTED_TREE_MODEL_NEW_HPP

#include "core_data_model/abstract_item_model.hpp"


/**
 *	Construct a tree data model by reflecting over the given objects.
 *	Tree may have zero or multiple "root" objects.
 */
class ReflectedTreeModelNew
	: public AbstractTreeModel
{
public:

	ReflectedTreeModelNew( int columnCount = 1 );
	virtual ~ReflectedTreeModelNew();
	

	virtual AbstractItem * item( const ItemIndex & index ) const override;
	virtual ItemIndex index( const AbstractItem * item ) const override;

	/**
	 *	Get the number of child items, excluding hidden items, including null items
	 *	E.g.
	 *	> group1
	 *	>> group2 <- hidden
	 *	>>> item1 - count
	 *	>>> item2 - count
	 *	>>> item3 - count
	 *	>> item4 - count
	 *	getChildCount( group1 ) == 4
	 *	E.g.
	 *	> group1
	 *	>> group2 <- hidden
	 *	>> group3 <- hidden
	 *	getChildCount( group1 ) == 0
	 */
	virtual int rowCount( const AbstractItem * item ) const override;
	virtual int columnCount() const override;

	virtual void addRootItem( AbstractItem * item );
	virtual void removeRootItem( AbstractItem * item );

	virtual Connection connectPreItemDataChanged( DataCallback callback ) override;
	virtual Connection connectPostItemDataChanged( DataCallback callback ) override;
	virtual Connection connectPreRowsInserted( RangeCallback callback ) override;
	virtual Connection connectPostRowsInserted( RangeCallback callback ) override;
	virtual Connection connectPreRowsRemoved( RangeCallback callback ) override;
	virtual Connection connectPostRowsRemoved( RangeCallback callback ) override;
	virtual Connection connectPreColumnsInserted( RangeCallback callback ) override;
	virtual Connection connectPostColumnsInserted( RangeCallback callback ) override;
	virtual Connection connectPreColumnsRemoved( RangeCallback callback ) override;
	virtual Connection connectPostColumnRemoved( RangeCallback callback ) override;

private:
	class Implementation;
	std::unique_ptr< Implementation > impl_;
};


#endif // _REFLECTED_TREE_MODEL_NEW_HPP
