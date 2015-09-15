#ifndef I_ITEM_FILTER_HPP
#define I_ITEM_FILTER_HPP

#include <vector>

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
};

#endif // I_ITEM_FILTER_HPP
