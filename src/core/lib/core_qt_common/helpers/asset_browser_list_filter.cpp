#include "asset_browser_list_filter.hpp"

#include "core_data_model/filtered_list_model.hpp"
#include "core_data_model/i_item.hpp"
#include "core_dependency_system/di_ref.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_qt_common/interfaces/i_check_filter.hpp"

#include <QRegExp>
#include <iostream>
#include <sstream>

struct AssetBrowserListFilter::Implementation
{
	Implementation( AssetBrowserListFilter & self );

	void setSource( IListModel * source );
	void setFilter( const QString & filter );
	bool checkFilter( const IItem* item );

	std::vector< QString > filterTokens_;
	AssetBrowserListFilter & self_;
	IListModel * source_;
	QString filter_;
	ICheckFilter* checkFilterInterface_;
	std::shared_ptr< FilteredListModel > filteredSource_;
};

AssetBrowserListFilter::Implementation::Implementation( AssetBrowserListFilter & self )
	: self_( self )
	, source_( nullptr )
	, checkFilterInterface_( nullptr )
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
		checkFilterInterface_ = Context::queryInterface< ICheckFilter >();

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
	if (filter_.compare( filter, Qt::CaseInsensitive ) == 0)
	{
		return;
	}

	filter_ = filter;

	// Tokenize the filter
	// TODO: Remove this basic tokenization functionality and set the filter to be an ObjectHandle of a 
	//       generic list of strings to support the WGActiveFilters control.
	// JIRA: http://jira.bigworldtech.com/browse/NGT-1009
	filterTokens_.clear();
	std::istringstream stream( filter_.toUtf8().constData() );
	std::string token;
	while (std::getline( stream, token, ' ' ))
	{
		if (token.length() > 0)
		{
			filterTokens_.push_back( QString::fromStdString( token ) );
		}
	}
	// End Tokenization

	if (filteredSource_ != nullptr)
	{
		filteredSource_->refresh();
	}

	emit self_.filterChanged();
}

bool AssetBrowserListFilter::Implementation::checkFilter( const IItem * item )
{
	if (filterTokens_.size() < 1)
	{
		return true;
	}

	// See if a custom filter is available
	if (nullptr != checkFilterInterface_)
	{
		return checkFilterInterface_->checkFilter( item, filterTokens_ );
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
