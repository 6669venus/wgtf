#include "wg_tree_filter.hpp"
#include "core_data_model/filtered_tree_model.hpp"
#include "core_data_model/i_item.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_reflection/object_handle.hpp"

#include <QRegExp>

struct WGTreeFilter::Implementation
{
	Implementation( WGTreeFilter & self );

	void setSource( ITreeModel * source );
	void setFilter( const QString & filter );
	bool checkFilter( const IItem* item );

	WGTreeFilter & self_;
	ITreeModel * source_;
	QString filter_;
	std::unique_ptr< FilteredTreeModel > filteredSource_;
};

WGTreeFilter::Implementation::Implementation( WGTreeFilter & self )
	: self_( self )
	, source_( nullptr )
{
}

void WGTreeFilter::Implementation::setSource( ITreeModel * source )
{
	if (source_ == source)
	{
		return;
	}

	source_ = source;
	filteredSource_ = nullptr;
	if (source_ != nullptr)
	{
		auto filterFunction = std::bind(
			&WGTreeFilter::Implementation::checkFilter,
			this,
			std::placeholders::_1 );
		filteredSource_ = std::unique_ptr< FilteredTreeModel >(
			new FilteredTreeModel( *source_, filterFunction ) );
	}
	emit self_.sourceChanged();
}

void WGTreeFilter::Implementation::setFilter( const QString & filter )
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

bool WGTreeFilter::Implementation::checkFilter( const IItem* item )
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


WGTreeFilter::WGTreeFilter()
	: impl_( new Implementation( *this ) )
{}

WGTreeFilter::~WGTreeFilter()
{}

QVariant WGTreeFilter::getSource() const
{
	if (impl_->source_ == nullptr)
	{
		return QVariant( QVariant::Invalid );
	}

	Variant variant = ObjectHandle( 
		const_cast< ITreeModel * >( impl_->source_ ) );
	return QtHelpers::toQVariant( variant );
}

QString WGTreeFilter::getFilter() const
{
	return impl_->filter_;
}

QVariant WGTreeFilter::getFilteredSource() const
{
	if (impl_->filteredSource_ == nullptr)
	{
		return getSource();
	}

	Variant variant = ObjectHandle( 
		static_cast< ITreeModel * >( impl_->filteredSource_.get() ) );
	return QtHelpers::toQVariant( variant );
}

void WGTreeFilter::setSource( const QVariant & source )
{
	Variant variant = QtHelpers::toVariant( source );
	if (variant.typeIs< ObjectHandle >())
	{
		ObjectHandle provider;
		if (variant.tryCast( provider ))
		{
			auto treeModel = provider.getBase< ITreeModel >();
			if (treeModel != nullptr)
			{
				impl_->setSource( treeModel );
			}
		}
	}
}

void WGTreeFilter::setFilter( const QString & filter )
{
	impl_->setFilter( filter );
}
