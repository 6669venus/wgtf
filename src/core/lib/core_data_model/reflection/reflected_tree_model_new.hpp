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

	ReflectedTreeModelNew();
	virtual ~ReflectedTreeModelNew();
	

	virtual AbstractItem * item( const ItemIndex & index ) const override;
	virtual ItemIndex index( const AbstractItem * item ) const override;

	/**
	 *	Check if tree has child items, excluding hidden items, including null items
	 *	E.g.
	 *	> group1
	 *	>> group2 <- hidden
	 *	>>> item1 - count
	 *	>>> item2 - count
	 *	>>> item3 - count
	 *	hasChildren( group1 ) == true
	 *	E.g.
	 *	> group1
	 *	>> group2 <- hidden
	 *	>> group3 <- hidden
	 *	hasChildren( group1 ) == false
	 */
	virtual int rowCount( const AbstractItem * item ) const override;
	virtual int columnCount() const override;

	void addRootItem( AbstractItem * item );
	void removeRootItem( AbstractItem * item );

private:
	class Implementation;
	std::unique_ptr< Implementation > impl_;
};


#endif // _REFLECTED_TREE_MODEL_NEW_HPP
