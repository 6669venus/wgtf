#include "string_filter.hpp"
#include "../i_item.hpp"
#include "../i_item_role.hpp"

struct StringFilter::Implementation
{
	Implementation( StringFilter & self );
	
	StringFilter & self_;
	std::string filterText_;
	unsigned int roleId_;
};

StringFilter::Implementation::Implementation( StringFilter & self )
	: self_( self )
	, filterText_( "" )
	, roleId_( ValueRole::roleId_ )
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

void StringFilter::setRole( unsigned int roleId )
{
	impl_->roleId_ = roleId;
}

bool StringFilter::checkFilter( const IItem* item )
{
	if (impl_->filterText_ == "")
	{
		return true;
	}
	
	if (item->columnCount() >= 0)
	{			
		std::string haystack = "";
		if (impl_->roleId_ == 0)
		{
			haystack = item->getDisplayText( 0 );
		}
		else 
		{
			auto data = item->getData( 0, impl_->roleId_ );
			bool result = data.tryCast( haystack );
			if (!result)
			{
				// The developer should provide a roleId that corresponds to string data
				return false;
			}
		}
			
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
