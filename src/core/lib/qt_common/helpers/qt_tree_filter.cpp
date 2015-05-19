#include "qt_tree_filter.hpp"
#include "data_model/filtered_tree_model.hpp"
#include "data_model/i_item.hpp"
#include "qt_common/helpers/qt_helpers.hpp"
#include "reflection/object_handle.hpp"

#include <QRegExp>

struct QtTreeFilter::Implementation
{
	Implementation( QtTreeFilter & self );

	void setSource( ITreeModel * source );
	void setFilter( const QString & filter );
	bool checkFilter( const IItem* item );

	QtTreeFilter & self_;
	ITreeModel * source_;
	QString filter_;
	std::unique_ptr< FilteredTreeModel > filteredSource_;
};

QtTreeFilter::Implementation::Implementation( QtTreeFilter & self )
	: self_( self )
	, source_( nullptr )
{
}

void QtTreeFilter::Implementation::setSource( ITreeModel * source )
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
			&QtTreeFilter::Implementation::checkFilter,
			this,
			std::placeholders::_1 );
		filteredSource_ = std::unique_ptr< FilteredTreeModel >(
			new FilteredTreeModel( *source_, filterFunction ) );
	}
	emit self_.sourceChanged();
}

void QtTreeFilter::Implementation::setFilter( const QString & filter )
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

bool QtTreeFilter::Implementation::checkFilter( const IItem* item )
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


QtTreeFilter::QtTreeFilter()
	: impl_( new Implementation( *this ) )
{}

QtTreeFilter::~QtTreeFilter()
{}

QVariant QtTreeFilter::getSource() const
{
	if (impl_->source_ == nullptr)
	{
		return QVariant( QVariant::Invalid );
	}

	Variant variant = ObjectHandle( 
		const_cast< ITreeModel * >( impl_->source_ ) );
	return QtHelpers::toQVariant( variant );
}

QString QtTreeFilter::getFilter() const
{
	return impl_->filter_;
}

QVariant QtTreeFilter::getFilteredSource() const
{
	if (impl_->filteredSource_ == nullptr)
	{
		return getSource();
	}

	Variant variant = ObjectHandle( 
		const_cast< FilteredTreeModel * >( impl_->filteredSource_.get() ) );
	return QtHelpers::toQVariant( variant );
}

void QtTreeFilter::setSource( const QVariant & source )
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

void QtTreeFilter::setFilter( const QString & filter )
{
	impl_->setFilter( filter );
}
