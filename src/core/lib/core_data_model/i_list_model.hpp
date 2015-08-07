#ifndef I_LIST_MODEL_HPP
#define I_LIST_MODEL_HPP

#include "wg_types/event.hpp"


class IItem;
class Variant;


/**
 *	Interface for accessing data as a "list".
 *	Note that this is not the same as the concept as a list in Qt. Qt counts
 *	list models as one-dimensional, only column 0 exists.
 *	The IListModel interface can have any number of columns.
 */
class IListModel
{
public:
	virtual ~IListModel() {}

	virtual IItem * item( size_t index ) const = 0;
	virtual size_t index( const IItem * item ) const = 0;
	virtual IItem * findItemByData(const Variant & data) const { return nullptr; }

	virtual bool empty() const = 0;
	virtual size_t size() const = 0;

	/**
	 * Check if this model has a clear implementation.
	 * Not all models can be cleared, some are static once they are created.
	 */
	virtual bool canClear() const { return false; }

	/**
	 * Try to clear the model.
	 * Should only work if canClear() returns true.
	 */
	virtual void clear() {}

	// TODO Need to remove IItem from all these arguments
	// because list models do not have parent items
	PUBLIC_EVENT( IListModel, PreDataChanged, 
		const IItem *, item, int, column, size_t, roleId, const Variant &, data )

	PUBLIC_EVENT( IListModel, PostDataChanged, 
		const IItem *, item, int, column, size_t, roleId, const Variant &, data )

	PUBLIC_EVENT( IListModel, PreItemsInserted,
		const IItem *, item, size_t, index, size_t, count )

	PUBLIC_EVENT( IListModel, PostItemsInserted,
		const IItem *, item, size_t, index, size_t, count )

	PUBLIC_EVENT( IListModel, PreItemsRemoved,
		const IItem *, item, size_t, index, size_t, count )

	PUBLIC_EVENT( IListModel, PostItemsRemoved,
		const IItem *, item, size_t, index, size_t, count )
};

#endif // I_LIST_MODEL_HPP
