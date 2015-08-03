#include "wg_list_filter.hpp"

#include "data_model/filtered_list_model.hpp"
#include "data_model/i_item.hpp"
#include "qt_common/helpers/qt_helpers.hpp"
#include "reflection/object_handle.hpp"

#include "interfaces/i_check_filter.hpp"

#include <QRegExp>

struct WGListFilter::Implementation
{
	Implementation( WGListFilter & self );

	void setSource( IListModel * source );
	void setFilter( const QString & filter );
	bool checkFilter( const IItem* item );

	WGListFilter & self_;
	IListModel * source_;
	QString filter_;
	std::shared_ptr< FilteredListModel > filteredSource_;
};

WGListFilter::Implementation::Implementation( WGListFilter & self )
	: self_( self )
	, source_( nullptr )
{
}

void WGListFilter::Implementation::setSource( IListModel * source )
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
			&WGListFilter::Implementation::checkFilter,
			this,
			std::placeholders::_1 );
		filteredSource_ = std::unique_ptr< FilteredListModel >(
			new FilteredListModel( *source_, filterFunction ) );
	}
	emit self_.sourceChanged();
}

void WGListFilter::Implementation::setFilter( const QString & filter )
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

bool WGListFilter::Implementation::checkFilter( const IItem* item )
{
	if (filter_ == "")
	{
		return true;
	}

	QRegExp reg( filter_ );

	if (item->columnCount() >= 0)
	{
		QString text = item->getDisplayText( 0 );

		if (reg.indexIn( text ) != -1)
		{
			return true;
		}
	}

	return false;
}


WGListFilter::WGListFilter()
	: impl_( new Implementation( *this ) )
{}

WGListFilter::~WGListFilter()
{}

QVariant WGListFilter::getSource() const
{
	if (impl_->source_ == nullptr)
	{
		return QVariant( QVariant::Invalid );
	}

	Variant variant = ObjectHandle( const_cast< IListModel * >( impl_->source_ ) );
	return QtHelpers::toQVariant( variant );
}

QString WGListFilter::getFilter() const
{
	return impl_->filter_;
}

QVariant WGListFilter::getFilteredSource() const
{
	if (impl_->filteredSource_ == nullptr)
	{
		return getSource();
	}

	Variant variant = ObjectHandle( 
		const_cast< FilteredListModel * >( impl_->filteredSource_.get() ) );
	return QtHelpers::toQVariant( variant );
}

void WGListFilter::setSource( const QVariant & source )
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

void WGListFilter::setFilter( const QString & filter )
{
	impl_->setFilter( filter );
}
