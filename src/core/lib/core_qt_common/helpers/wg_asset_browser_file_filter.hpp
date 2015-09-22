#ifndef WG_ASSET_BROWSER_FILE_FILTER_HPP
#define WG_ASSET_BROWSER_FILE_FILTER_HPP

#include "../qt_new_handler.hpp"
#include "wg_filter.hpp"
#include <memory>

class QString;
class QVariant;

/**
 *	WGAssetBrowserFileFilter
 *	A specialized filter that extends functionality of the tokenized filter and pulls the data it needs
 *  for comparisons directly from the full file name of the asset browser file object model.
 *  
 *  TODO: Make a custom IItem whose getDisplayText() function will retrieve the IAssetObjectModel filename and use
 *        GenericListT<> instead of VariantList for the folder contents in the asset browser.
 *  JIRA: http://jira.bigworldtech.com/browse/NGT-1077
 */
class WGAssetBrowserFileFilter : public WGFilter
{
	Q_OBJECT

	Q_PROPERTY( QString filterText
				READ getFilterText
				WRITE setFilterText
				NOTIFY filterTextChanged )

	Q_PROPERTY( QString splitterChar
				READ getSplitterChar
				WRITE setSplitterChar
				NOTIFY splitterCharChanged )

	DECLARE_QT_MEMORY_HANDLER

public:
	WGAssetBrowserFileFilter();
	virtual ~WGAssetBrowserFileFilter();
	
	virtual IItemFilter * getFilter() const override;

private:	
	QString getFilterText() const;
	void setFilterText( const QString & filterText );

	QString getSplitterChar() const;
	void setSplitterChar( const QString & splitter );

signals:
	void filterTextChanged();
	void splitterCharChanged();

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif // WG_ASSET_BROWSER_FILE_FILTER_HPP
