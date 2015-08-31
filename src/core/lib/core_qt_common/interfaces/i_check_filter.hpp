#ifndef I_CHECK_FILTER_HPP
#define I_CHECK_FILTER_HPP

#include <vector>

/// Interface class for the QtListFilter's custom filter

class IItem;
class QString;

class ICheckFilter
{
public:

	virtual bool checkFilter( const IItem * item, const std::vector<QString> & filters ) = 0;
};

#endif // I_CHECK_FILTER_HPP