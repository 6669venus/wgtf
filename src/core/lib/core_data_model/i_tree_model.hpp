#ifndef I_TREE_MODEL_HPP
#define I_TREE_MODEL_HPP

#include "wg_types/event.hpp"
#include "i_item.hpp"


/**
 *	This is our generic data model of a tree.
 *	Composed of a collection of IItems.
 *	Data can be get/set.
 *	There is no concept of rows or columns like Qt. 
 */
class ITreeModel
{
public:
	typedef std::pair< size_t, const IItem * > ItemIndex;

	virtual ~ITreeModel()
	{
		notifyDestructing();
	}

	virtual IItem * item( size_t index, const IItem * parent ) const = 0;
	virtual IItem * item( ItemIndex index ) const;
	virtual ItemIndex index( const IItem * item ) const = 0;

	virtual bool empty( const IItem * item ) const;
	virtual size_t size( const IItem * item ) const = 0;

	PUBLIC_EVENT( ITreeModel, PreDataChanged, 
		const IItem *, item, int, column, uint64_t, roleId, const Variant &, data )

	PUBLIC_EVENT( ITreeModel, PostDataChanged, 
		const IItem *, item, int, column, uint64_t, roleId, const Variant &, data)

	PUBLIC_EVENT( ITreeModel, PreItemsInserted,
		const IItem *, item, size_t, index, size_t, count )

	PUBLIC_EVENT( ITreeModel, PostItemsInserted,
		const IItem *, item, size_t, index, size_t, count )

	PUBLIC_EVENT( ITreeModel, PreItemsRemoved,
		const IItem *, item, size_t, index, size_t, count )

	PUBLIC_EVENT( ITreeModel, PostItemsRemoved,
		const IItem *, item, size_t, index, size_t, count )

	PUBLIC_EVENT( ITreeModel, Destructing )

};

#endif // I_TREE_MODEL_HPP
