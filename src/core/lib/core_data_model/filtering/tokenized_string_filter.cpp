#include "tokenized_string_filter.hpp"
#include "../i_item.hpp"
#include <iostream>
#include <sstream>

struct TokenizedStringFilter::Implementation
{
	Implementation( TokenizedStringFilter & self );
	
	TokenizedStringFilter & self_;
	std::vector< std::string > filterTokens_;
	std::string sourceFilterText_;
	std::string splitter_;
};

TokenizedStringFilter::Implementation::Implementation( TokenizedStringFilter & self )
	: self_( self )
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

bool TokenizedStringFilter::checkFilter( const IItem* item )
{
	if (impl_->filterTokens_.size() < 1)
	{
		return true;
	}

	bool checkFilterPassed = true;

	for (auto & filter : impl_->filterTokens_)
	{	
		if (item->columnCount() >= 0)
		{
			std::string haystack = item->getDisplayText( 0 );
			
			std::transform( haystack.begin(), haystack.end(), haystack.begin(), ::tolower );
			std::transform( filter.begin(), filter.end(), filter.begin(), ::tolower );

			checkFilterPassed &= (haystack.find( filter ) != std::string::npos);
		}
	}

	return checkFilterPassed;
}
