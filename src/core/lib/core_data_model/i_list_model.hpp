#ifndef I_LIST_MODEL_HPP
#define I_LIST_MODEL_HPP

#include "core_common/signal.hpp"

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
	typedef Signal< void( const IItem *, int, size_t, const Variant & ) > SignalData;
	typedef Signal< void( size_t, size_t ) > SignalCount;
	typedef Signal< void( void ) > SignalVoid;

public:
	virtual ~IListModel()
	{
		signalDestructing();
	}

	virtual IItem * item( size_t index ) const = 0;
	virtual size_t index( const IItem * item ) const = 0;

	virtual bool empty() const = 0;
	virtual size_t size() const = 0;
	virtual int columnCount() const = 0;

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

	// IListModel signals
	SignalData signalPreDataChanged;
	SignalData signalPostDataChanged;
	SignalCount signalPreItemsInserted;
	SignalCount signalPostItemsInserted;
	SignalCount signalPreItemsRemoved;
	SignalCount signalPostItemsRemoved;
	SignalVoid signalDestructing;
};

#endif // I_LIST_MODEL_HPP
