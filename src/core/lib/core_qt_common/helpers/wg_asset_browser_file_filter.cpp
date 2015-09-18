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
	~Implementation();

	void setFilterText( const QString & filterText );
	void setSplitter( const QString & splitter );

	WGAssetBrowserFileFilter & self_;
	AssetBrowserFileFilter * filter_;
};

WGAssetBrowserFileFilter::Implementation::Implementation( WGAssetBrowserFileFilter & self )
	: self_( self )
	, filter_( new AssetBrowserFileFilter() )
{
}

WGAssetBrowserFileFilter::Implementation::~Implementation()
{
	if (filter_ != nullptr)
	{
		delete filter_;
		filter_ = nullptr;
	}
}

void WGAssetBrowserFileFilter::Implementation::setFilterText( const QString & filterText )
{
	if (filter_ == nullptr)
	{
		return;
	}

	std::string inputValue = filterText.toUtf8().constData();
	if (strcmp( inputValue.c_str(), filter_->getFilterText() ) == 0)
	{
		return;
	}

	filter_->updateFilterTokens( inputValue.c_str() );
	filter_->notifyFilterChanged();
}

void WGAssetBrowserFileFilter::Implementation::setSplitter( const QString & splitter )
{
	if (filter_ == nullptr)
	{
		return;
	}

	std::string inputValue = splitter.toUtf8().constData();
	if (strcmp( inputValue.c_str(), filter_->getSplitterChar() ) == 0)
	{
		return;
	}

	filter_->setSplitterChar( inputValue.c_str() );
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
	if (impl_->filter_ != nullptr)
	{
		return QString::fromStdString( impl_->filter_->getFilterText() );
	}

	return "";
}

void WGAssetBrowserFileFilter::setFilterText( const QString & filterText )
{
	impl_->setFilterText( filterText );
}

IItemFilter * WGAssetBrowserFileFilter::getFilter() const
{
	return static_cast< IItemFilter * >( impl_->filter_ );
}

QString WGAssetBrowserFileFilter::getSplitterChar() const
{
	if (impl_->filter_ != nullptr)
	{
		return QString::fromStdString( impl_->filter_->getSplitterChar() );
	}

	return "";
}

void WGAssetBrowserFileFilter::setSplitterChar( const QString & splitter )
{
	impl_->setSplitter( splitter );
	emit splitterCharChanged();
}

