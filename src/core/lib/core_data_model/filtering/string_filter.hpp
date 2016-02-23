#ifndef STRING_FILTER_HPP
#define STRING_FILTER_HPP

#include "i_item_filter.hpp"
#include <memory>

/**
 *	StringFilter
 *  A simple string filter implementation.
 */
class StringFilter : public IItemFilter
{
public:
	StringFilter();
	virtual ~StringFilter();
	
	virtual bool checkFilter( const IItem * item ) override;

	virtual void setRole( unsigned int roleId ) override;

	void setFilterText( const char * filterText );
	const char* getFilterText();

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif // STRING_FILTER_HPP
