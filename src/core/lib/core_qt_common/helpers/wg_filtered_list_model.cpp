#include "wg_filtered_list_model.hpp"

#include "core_data_model/filtered_list_model.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/filtering/i_item_filter.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_qt_common/helpers/wg_filter.hpp"
#include "core_qt_common/qt_connection_holder.hpp"
#include "core_reflection/object_handle.hpp"

#include <QRegExp>

struct WGFilteredListModel::Implementation
{
	Implementation( WGFilteredListModel & self );

	void setFilter( WGFilter * filter );
	void onFilterChanged( const IItemFilter* sender, const IItemFilter::FilterChangedArgs& args );

	WGFilteredListModel & self_;
	WGFilter * filter_;
	FilteredListModel filteredModel_;
	QtConnectionHolder connections_;
};

WGFilteredListModel::Implementation::Implementation( WGFilteredListModel & self )
	: self_( self )
	, filter_( nullptr )
{
}

void WGFilteredListModel::Implementation::setFilter( WGFilter * filter )
{
	if (filter_ == filter)
	{
		return;
	}

	IItemFilter * current = filter_ != nullptr ? filter_->getFilter() : nullptr;
	if (current != nullptr)
	{
		current->onFilterChanged().remove< WGFilteredListModel::Implementation,
			&WGFilteredListModel::Implementation::onFilterChanged >( this );
	}

	filter_ = filter;
	current = filter_ != nullptr ? filter_->getFilter() : nullptr;

	if (current != nullptr)
	{
		current->onFilterChanged().add< WGFilteredListModel::Implementation, 
			&WGFilteredListModel::Implementation::onFilterChanged >( this );
	}

	filteredModel_.setFilter( current );
	emit self_.filterChanged();
}

void WGFilteredListModel::Implementation::onFilterChanged( const IItemFilter* sender, 
														   const IItemFilter::FilterChangedArgs& args )
{
	if (sender != filter_->getFilter())
	{
		// This isn't the filter bound to this component
		return;
	}

	filteredModel_.refresh();
}

WGFilteredListModel::WGFilteredListModel()
	: impl_( new Implementation( *this ) )
{
	impl_->connections_ += QObject::connect( 
		this, &WGListModel::sourceChanged, 
		this, &WGFilteredListModel::onSourceChanged ); 
}

WGFilteredListModel::~WGFilteredListModel()
{
	// Temporary hack to circumvent threading deadlock
	// JIRA: http://jira.bigworldtech.com/browse/NGT-227
	impl_->filteredModel_.setSource( nullptr );
	// End temporary hack

	impl_->setFilter( nullptr );
}

IListModel * WGFilteredListModel::getModel() const 
{
	// This component will return the filtered source, not the original source.
	return &impl_->filteredModel_;
}

void WGFilteredListModel::onSourceChanged()
{
	IListModel * source = nullptr;

	Variant variant = QtHelpers::toVariant( getSource() );
	if (variant.typeIs< ObjectHandle >())
	{
		ObjectHandle provider;
		if (variant.tryCast( provider ))
		{
			source = provider.getBase< IListModel >();
		}
	}

	impl_->filteredModel_.setSource( source );
}

QObject * WGFilteredListModel::getFilter() const
{
	return impl_->filter_;
}

void WGFilteredListModel::setFilter( QObject * filter )
{
	auto wgFilter = qobject_cast< WGFilter * >( filter );
	impl_->setFilter( wgFilter );
}

