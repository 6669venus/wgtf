#ifndef ASSET_BROWSER_LIST_CUSTOM_FILTER_HPP
#define ASSET_BROWSER_LIST_CUSTOM_FILTER_HPP

/// AssetBrowser's custom filter class for QtListFilter.

#include "core_qt_common/interfaces/i_check_filter.hpp"
#include "core_dependency_system/i_interface.hpp"

class AssetBrowserListCustomFilter : 
	public Implements< ICheckFilter >
{
public:

	AssetBrowserListCustomFilter();
	virtual ~AssetBrowserListCustomFilter();

	virtual bool checkFilter( const IItem * item, const std::vector<QString> & filters );
};

#endif // ASSET_BROWSER_LIST_CUSTOM_FILTER_HPP