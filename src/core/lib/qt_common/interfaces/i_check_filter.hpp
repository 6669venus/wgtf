#ifndef I_CHECK_FILTER_HPP
#define I_CHECK_FILTER_HPP

/// Interface class for the QtListFilter's custom filter

class IItem;
class QString;

class ICheckFilter
{
public:

	virtual bool checkFilter( const IItem * item, const QString & filter ) = 0;
};

#endif // I_CHECK_FILTER_HPP