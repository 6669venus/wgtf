#include "qt_list_filter.hpp"

#include "data_model/filtered_list_model.hpp"
#include "data_model/i_item.hpp"
#include "qt_common/helpers/qt_helpers.hpp"
#include "reflection/object_handle.hpp"

#include "interfaces/i_check_filter.hpp"

#include <QRegExp>

struct QtListFilter::Implementation
{
	Implementation( QtListFilter & self );

	void setSource( IListModel * source );
	void setFilter( const QString & filter );
	bool checkFilter( const IItem* item );

	QtListFilter & self_;
	IListModel * source_;
	QString filter_;
	std::shared_ptr< FilteredListModel > filteredSource_;
};

QtListFilter::Implementation::Implementation( QtListFilter & self )
	: self_( self )
	, source_( nullptr )
{
}

void QtListFilter::Implementation::setSource( IListModel * source )
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
			&QtListFilter::Implementation::checkFilter,
			this,
			std::placeholders::_1 );
		filteredSource_ = std::unique_ptr< FilteredListModel >(
			new FilteredListModel( *source_, filterFunction ) );
	}
	emit self_.sourceChanged();
}

void QtListFilter::Implementation::setFilter( const QString & filter )
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

bool QtListFilter::Implementation::checkFilter( const IItem* item )
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


QtListFilter::QtListFilter()
	: impl_( new Implementation( *this ) )
{}

QtListFilter::~QtListFilter()
{}

QVariant QtListFilter::getSource() const
{
	if (impl_->source_ == nullptr)
	{
		return QVariant( QVariant::Invalid );
	}

	Variant variant = ObjectHandle( const_cast< IListModel * >( impl_->source_ ) );
	return QtHelpers::toQVariant( variant );
}

QString QtListFilter::getFilter() const
{
	return impl_->filter_;
}

QVariant QtListFilter::getFilteredSource() const
{
	if (impl_->filteredSource_ == nullptr)
	{
		return getSource();
	}

	Variant variant = ObjectHandle( 
		const_cast< FilteredListModel * >( impl_->filteredSource_.get() ) );
	return QtHelpers::toQVariant( variant );
}

void QtListFilter::setSource( const QVariant & source )
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

void QtListFilter::setFilter( const QString & filter )
{
	impl_->setFilter( filter );
}
