#include "tokenized_string_filter.hpp"
#include "../i_item.hpp"
#include "../i_item_role.hpp"
#include <iostream>
#include <sstream>

struct TokenizedStringFilter::Implementation
{
	Implementation( TokenizedStringFilter & self );
	
	TokenizedStringFilter & self_;
	std::vector< std::string > filterTokens_;
	std::string sourceFilterText_;
	std::string splitter_;
	unsigned int roleId_;
};

TokenizedStringFilter::Implementation::Implementation( TokenizedStringFilter & self )
	: self_( self )
	, sourceFilterText_( "" )
	, splitter_( " " )
	, roleId_( ValueRole::roleId_ )
{
}

TokenizedStringFilter::TokenizedStringFilter()
	: impl_( new Implementation( *this ) )
{
}

TokenizedStringFilter::~TokenizedStringFilter()
{
}

void TokenizedStringFilter::updateFilterTokens( const char * filterText )
{
	impl_->sourceFilterText_ = filterText;

	impl_->filterTokens_.clear();

	std::istringstream stream( filterText );
	std::string token;

	char splitter = ' ';
	if (impl_->splitter_.length() > 0)
	{
		splitter = impl_->splitter_[0];
	}
		
	while (std::getline( stream, token, splitter ))
	{
		if (token.length() > 0)
		{
			impl_->filterTokens_.push_back( token );
		}
	}
}

const char* TokenizedStringFilter::getFilterText()
{
	return impl_->sourceFilterText_.c_str();
}

void TokenizedStringFilter::setSplitterChar( const char * splitter )
{
	impl_->splitter_ = splitter;
}

const char* TokenizedStringFilter::getSplitterChar()
{
	return impl_->splitter_.c_str();
}

void TokenizedStringFilter::setRole( unsigned int roleId )
{
	impl_->roleId_ = roleId;
}

bool TokenizedStringFilter::checkFilter( const IItem* item )
{
	if (impl_->filterTokens_.size() < 1)
	{
		return true;
	}

	bool checkFilterPassed = true;

	
	std::string haystack = "";
	if (item->columnCount() >= 0)
	{
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
	}
	
	std::transform( haystack.begin(), haystack.end(), haystack.begin(), ::tolower );

	for (auto & filter : impl_->filterTokens_)
	{				
		std::transform( filter.begin(), filter.end(), filter.begin(), ::tolower );
		checkFilterPassed &= (haystack.find( filter ) != std::string::npos);
	}

	return checkFilterPassed;
}
