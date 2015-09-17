#include "string_filter.hpp"
#include "../i_item.hpp"

struct StringFilter::Implementation
{
	Implementation( StringFilter & self );
	
	StringFilter & self_;
	std::string filterText_;
};

StringFilter::Implementation::Implementation( StringFilter & self )
	: self_( self )
	, filterText_( "" )
{
}

StringFilter::StringFilter()
	: impl_( new Implementation( *this ) )
{
}

StringFilter::~StringFilter()
{
}

void StringFilter::setFilterText( const char * filterText )
{
	impl_->filterText_ = filterText;
}

const char* StringFilter::getFilterText()
{
	return impl_->filterText_.c_str();
}

bool StringFilter::checkFilter( const IItem* item )
{
	if (impl_->filterText_ == "")
	{
		return true;
	}
	
	if (item->columnCount() >= 0)
	{
		std::string haystack = item->getDisplayText( 0 );
			
		std::transform( haystack.begin(), haystack.end(), haystack.begin(), ::tolower );

		std::string filter = impl_->filterText_;
		std::transform( filter.begin(), filter.end(), filter.begin(), ::tolower );

		if (haystack.find( filter ) != std::string::npos)
		{
			return true;
		}
	}

	return false;
}
