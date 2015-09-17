#ifndef ASSET_BROWSER_FILE_FILTER_HPP
#define ASSET_BROWSER_FILE_FILTER_HPP

#include "wg_types/event.hpp"
#include "i_item_filter.hpp"
#include <memory>

/**
 *	AssetBrowserFileFilter
 *  A specialized filter to look at FileObjectModel data and determine if the file's name meets requirements
 *  per the tokenized filter. 
 *
 *  TODO: This is a temporary measure to introduce filtering using the new pattern
 *  until the folder contents list can be changed to use a GenericListT<> with a custom IItem whose getDisplayName
 *  function returns the file name.
 *
 *  JIRA: http://jira.bigworldtech.com/browse/NGT-1077
 *  
 */
class AssetBrowserFileFilter : public IItemFilter
{
public:
	AssetBrowserFileFilter();
	virtual ~AssetBrowserFileFilter();
	
	virtual bool checkFilter( const IItem * item ) override;

	void updateFilterTokens( const char * filterText );
	const char* getFilterText();

	void setSplitterChar( const char * splitter );
	const char* getSplitterChar();

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif // ASSET_BROWSER_FILE_FILTER_HPP
