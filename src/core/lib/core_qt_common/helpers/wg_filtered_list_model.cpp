#include "wg_filtered_list_model.hpp"

#include "core_data_model/filtered_list_model.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/filtering/i_item_filter.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_reflection/object_handle.hpp"

#include <QRegExp>

struct WGFilteredListModel::Implementation
{
	Implementation( WGFilteredListModel & self );

	void updateSource();
	void setFilter( IItemFilter * filter );
	void onFilterChanged( const IItemFilter* sender, const IItemFilter::FilterChangedArgs& args );

	WGFilteredListModel & self_;
	IItemFilter * filter_;
	FilteredListModel filteredSource_;
};

WGFilteredListModel::Implementation::Implementation( WGFilteredListModel & self )
	: self_( self )
{
}

void WGFilteredListModel::Implementation::updateSource()
{
	filteredSource_.setSource( self_.source() );
}

void WGFilteredListModel::Implementation::setFilter( IItemFilter * filter )
{
	if (filter_ == filter)
	{
		return;
	}

	filter_ = filter;
	filter_->onFilterChanged().add< WGFilteredListModel::Implementation, 
		&WGFilteredListModel::Implementation::onFilterChanged >( this );

	filteredSource_.setFilter( filter );
	emit self_.filterChanged();
}

void WGFilteredListModel::Implementation::onFilterChanged( const IItemFilter* sender, 
														   const IItemFilter::FilterChangedArgs& args )
{
	if (sender != filter_)
	{
		// This isn't the filter bound to this component
		return;
	}

	filteredSource_.refresh();
}

WGFilteredListModel::WGFilteredListModel()
	: impl_( new Implementation( *this ) )
{
	QObject::connect( 
		this, &WGListModel::sourceChanged, this, &WGFilteredListModel::onSourceChanged ); 
}

WGFilteredListModel::~WGFilteredListModel()
{
}

IListModel * WGFilteredListModel::getModel() const 
{
	// This component will return the filtered source, not the original source.
	return &impl_->filteredSource_;
}

void WGFilteredListModel::onSourceChanged()
{
	impl_->filteredSource_.setSource( source() );
}

QVariant WGFilteredListModel::getFilter() const
{
	Variant variant = ObjectHandle( const_cast< IItemFilter * >( impl_->filter_ ) );
	return QtHelpers::toQVariant( variant );
}

void WGFilteredListModel::setFilter( const QVariant & filter )
{
	Variant variant = QtHelpers::toVariant( filter );
	if (variant.typeIs< ObjectHandle >())
	{
		ObjectHandle provider;
		if (variant.tryCast( provider ))
		{
			auto itemFilter = provider.getBase< IItemFilter >();
			if ( nullptr != itemFilter )
			{
				impl_->setFilter( itemFilter );
			}
		}
	}
}

