#include "wg_asset_browser_file_filter.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/filtering/asset_browser_file_filter.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include <QString>
#include <QVariant>

struct WGAssetBrowserFileFilter::Implementation
{
	Implementation( WGAssetBrowserFileFilter & self );

	void setFilterText( const QString & filterText );
	void setSplitter( const QString & splitter );

	WGAssetBrowserFileFilter & self_;
	AssetBrowserFileFilter filter_;
};

WGAssetBrowserFileFilter::Implementation::Implementation( WGAssetBrowserFileFilter & self )
	: self_( self )
{
}

void WGAssetBrowserFileFilter::Implementation::setFilterText( const QString & filterText )
{
	std::string inputValue = filterText.toUtf8().constData();
	if (strcmp( inputValue.c_str(), filter_.getFilterText() ) == 0)
	{
		return;
	}

	filter_.updateFilterTokens( inputValue.c_str() );
	filter_.notifyFilterChanged();
}

void WGAssetBrowserFileFilter::Implementation::setSplitter( const QString & splitter )
{
	std::string inputValue = splitter.toUtf8().constData();
	if (strcmp( inputValue.c_str(), filter_.getSplitterChar() ) == 0)
	{
		return;
	}

	filter_.setSplitterChar( inputValue.c_str() );
}

WGAssetBrowserFileFilter::WGAssetBrowserFileFilter()
	: impl_( new Implementation( *this ) )
{
}

WGAssetBrowserFileFilter::~WGAssetBrowserFileFilter()
{
}

QString WGAssetBrowserFileFilter::getFilterText() const
{
	return QString::fromStdString( impl_->filter_.getFilterText() );
}

void WGAssetBrowserFileFilter::setFilterText( const QString & filterText )
{
	impl_->setFilterText( filterText );
}

IItemFilter * WGAssetBrowserFileFilter::getFilter() const
{
	return static_cast< IItemFilter * >( &impl_->filter_ );
}

QString WGAssetBrowserFileFilter::getSplitterChar() const
{
	return QString::fromStdString( impl_->filter_.getSplitterChar() );
}

void WGAssetBrowserFileFilter::setSplitterChar( const QString & splitter )
{
	impl_->setSplitter( splitter );
	emit splitterCharChanged();
}

