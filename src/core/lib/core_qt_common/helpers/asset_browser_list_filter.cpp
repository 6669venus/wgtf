#include "asset_browser_list_filter.hpp"

#include "core_data_model/filtered_list_model.hpp"
#include "core_data_model/i_item.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_reflection/object_handle.hpp"

#include "core_qt_common/interfaces/i_check_filter.hpp"

#include <QRegExp>

struct AssetBrowserListFilter::Implementation
{
	Implementation( AssetBrowserListFilter & self );

	void setSource( IListModel * source );
	void setFilter( const QString & filter );
	bool checkFilter( const IItem* item );

	AssetBrowserListFilter & self_;
	IListModel * source_;
	QString filter_;
	std::shared_ptr< FilteredListModel > filteredSource_;
};

AssetBrowserListFilter::Implementation::Implementation( AssetBrowserListFilter & self )
	: self_( self )
	, source_( nullptr )
{
}

void AssetBrowserListFilter::Implementation::setSource( IListModel * source )
{
	if (source_ == source)
	{
		return;
	}

	source_ = source;
	filteredSource_ = nullptr;

	if (nullptr != source_)
	{
		auto filterFunction = std::bind(
			&AssetBrowserListFilter::Implementation::checkFilter,
			this,
			std::placeholders::_1 );
		filteredSource_ = std::unique_ptr< FilteredListModel >(
			new FilteredListModel( *source_, filterFunction ) );
	}
	emit self_.sourceChanged();
}

void AssetBrowserListFilter::Implementation::setFilter( const QString & filter )
{
	if (filter_ == filter)
	{
		return;
	}

	filter_ = filter;
	if (filteredSource_ != nullptr)
	{
		filteredSource_->refresh();
	}
	emit self_.filterChanged();
}

bool AssetBrowserListFilter::Implementation::checkFilter( const IItem * item )
{
	if (filter_ == "")
	{
		return true;
	}

	// See if a custom filter is available
	ICheckFilter * checkFilterInterface = Context::queryInterface< ICheckFilter >();
	if (nullptr != checkFilterInterface)
	{
		return checkFilterInterface->checkFilter( item, filter_ );
	}
	else
	{
		QRegExp reg( filter_ );

		if (item->columnCount() >= 0)
		{
			QString text = item->getDisplayText( 0 );

			if (reg.indexIn( text ) != -1)
			{
				return true;
			}
		}
	}

	return false;
}


AssetBrowserListFilter::AssetBrowserListFilter()
	: impl_( new Implementation( *this ) )
{}

AssetBrowserListFilter::~AssetBrowserListFilter()
{}

QVariant AssetBrowserListFilter::getSource() const
{
	if (impl_->source_ == nullptr)
	{
		return QVariant( QVariant::Invalid );
	}

	Variant variant = ObjectHandle( const_cast< IListModel * >( impl_->source_ ) );
	return QtHelpers::toQVariant( variant );
}

QString AssetBrowserListFilter::getFilter() const
{
	return impl_->filter_;
}

QVariant AssetBrowserListFilter::getFilteredSource() const
{
	if (impl_->filteredSource_ == nullptr)
	{
		return getSource();
	}

	Variant variant = ObjectHandle( 
		const_cast< FilteredListModel * >( impl_->filteredSource_.get() ) );
	return QtHelpers::toQVariant( variant );
}

void AssetBrowserListFilter::setSource( const QVariant & source )
{
	Variant variant = QtHelpers::toVariant( source );
	if (variant.typeIs< ObjectHandle >())
	{
		ObjectHandle provider;
		if (variant.tryCast( provider ))
		{
			auto listModel = provider.getBase< IListModel >();
			if ( nullptr != listModel )
			{
				impl_->setSource( listModel );
			}
		}
	}
}

void AssetBrowserListFilter::setFilter( const QString & filter )
{
	impl_->setFilter( filter );
}
