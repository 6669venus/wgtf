#ifndef I_ITEM_FILTER_HPP
#define I_ITEM_FILTER_HPP

#include "wg_types/event.hpp"

class IItem;

/**
 *	IItemFilter
 *  A filter interface that can be implemented to provide advanced
 *  filtering functionality to the IItem-driven QML filtering components and
 *  their corresponding data models.
 */
class IItemFilter
{
public:
	virtual ~IItemFilter() {}
	
	virtual bool checkFilter( const IItem * item ) = 0;

	virtual void setRole( unsigned int roleId ) = 0;

	virtual bool filterDescendantsOfMatchingItems() { return false; }

	PUBLIC_EVENT( IItemFilter, FilterChanged );
};

#endif // I_ITEM_FILTER_HPP
