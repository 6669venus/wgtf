#include "asset_browser_file_filter.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/asset_browser/file_object_model.hpp"
#include <iostream>
#include <mutex>
#include <sstream>

struct AssetBrowserFileFilter::Implementation
{
	Implementation( AssetBrowserFileFilter & self );
	
	AssetBrowserFileFilter & self_;
	std::vector< std::string > filterTokens_;
	std::mutex filterTokensLock_;
	std::string sourceFilterText_;
	std::string splitter_;
};

AssetBrowserFileFilter::Implementation::Implementation( AssetBrowserFileFilter & self )
	: self_( self )
	, sourceFilterText_( "" )
	, splitter_( " " )
{
}

AssetBrowserFileFilter::AssetBrowserFileFilter()
	: impl_( new Implementation( *this ) )
{
}

AssetBrowserFileFilter::~AssetBrowserFileFilter()
{
}

void AssetBrowserFileFilter::updateFilterTokens( const char * filterText )
{
	impl_->sourceFilterText_ = filterText;

	std::lock_guard< std::mutex >( impl_->filterTokensLock_ );
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
			std::transform( token.begin(), token.end(), token.begin(), ::tolower );
			impl_->filterTokens_.push_back( token );
		}
	}
}

const char* AssetBrowserFileFilter::getFilterText()
{
	return impl_->sourceFilterText_.c_str();
}

void AssetBrowserFileFilter::setSplitterChar( const char * splitter )
{
	impl_->splitter_ = splitter;
}

const char* AssetBrowserFileFilter::getSplitterChar()
{
	return impl_->splitter_.c_str();
}

bool AssetBrowserFileFilter::checkFilter( const IItem* item )
{
	if (impl_->filterTokens_.size() < 1)
	{
		return true;
	}

	std::string haystack = "";

	if (item->columnCount() >= 0)
	{
		ObjectHandle object;
		IItem * listItem = static_cast< IItem *>( const_cast< IItem * >( item ) );
		Variant itemData = listItem->getData( 0, ValueRole::roleId_ );

		if (itemData.tryCast( object ))
		{
			auto tmp = object.getBase< IAssetObjectModel >();
			if (tmp == nullptr)
			{
				// Invalid object
				return false;
			}

			haystack = tmp->getFileName();
		}
	}

	if (haystack.length() == 0)
	{
		return false;
	}

	std::transform( haystack.begin(), haystack.end(), haystack.begin(), ::tolower );
	std::lock_guard< std::mutex >( impl_->filterTokensLock_ );	

	for (auto & filter : impl_->filterTokens_)
	{	
		if (haystack.find( filter ) == std::string::npos)
		{
			return false;
		}
	}

	return true;
}
