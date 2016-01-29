#include "wg_filtered_tree_model.hpp"

#include "core_data_model/filtered_tree_model.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/filtering/i_item_filter.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_qt_common/helpers/wg_filter.hpp"
#include "core_qt_common/qt_connection_holder.hpp"
#include "core_reflection/object_handle.hpp"

#include <QRegExp>

struct WGFilteredTreeModel::Implementation
{
	Implementation( WGFilteredTreeModel & self );

	void setFilter( WGFilter * filter );
	void onFilterChanged( const IItemFilter* sender, const IItemFilter::FilterChangedArgs& args );

	WGFilteredTreeModel & self_;
	WGFilter * filter_;
	FilteredTreeModel filteredModel_;
	QtConnectionHolder connections_;
};

WGFilteredTreeModel::Implementation::Implementation( WGFilteredTreeModel & self )
	: self_( self )
	, filter_( nullptr )
{
}

void WGFilteredTreeModel::Implementation::setFilter( WGFilter * filter )
{
	if (filter_ == filter)
	{
		return;
	}

	IItemFilter * current = filter_ != nullptr ? filter_->getFilter() : nullptr;
	if (current != nullptr)
	{
		current->onFilterChanged().remove< WGFilteredTreeModel::Implementation,
			&WGFilteredTreeModel::Implementation::onFilterChanged >( this );
	}

	filter_ = filter;
	current = filter_ != nullptr ? filter_->getFilter() : nullptr;

	if (current != nullptr)
	{
		current->onFilterChanged().add< WGFilteredTreeModel::Implementation, 
			&WGFilteredTreeModel::Implementation::onFilterChanged >( this );
	}

	filteredModel_.setFilter( current );
	emit self_.filterChanged();
}

void WGFilteredTreeModel::Implementation::onFilterChanged( const IItemFilter* sender, 
														   const IItemFilter::FilterChangedArgs& args )
{
	if (filter_ != nullptr && sender != filter_->getFilter())
	{
		// This isn't the filter bound to this component
		return;
	}

	filteredModel_.refresh();
}

WGFilteredTreeModel::WGFilteredTreeModel()
	: impl_( new Implementation( *this ) )
{
	impl_->connections_ += QObject::connect( 
		this, &WGTreeModel::sourceChanged, 
		this, &WGFilteredTreeModel::onSourceChanged ); 
}

WGFilteredTreeModel::~WGFilteredTreeModel()
{
	setSource( QVariant() );

	impl_->connections_.reset();
	
	// Temporary hack to circumvent threading deadlock
	// JIRA: http://jira.bigworldtech.com/browse/NGT-227
	impl_->filteredModel_.setSource( nullptr );
	// End temporary hack

	impl_->setFilter( nullptr );

	// evgenys: reseting impl_ to null first to avoid pure virtual func call in filteredModel_ destructor
	delete impl_.release();
}

ITreeModel * WGFilteredTreeModel::getModel() const 
{
	// This component will return the filtered source, not the original source.
	return impl_ ? &impl_->filteredModel_ : nullptr;
}

void WGFilteredTreeModel::onSourceChanged()
{
	ITreeModel * source = nullptr;

	Variant variant = QtHelpers::toVariant( getSource() );
	if (variant.typeIs< ObjectHandle >())
	{
		ObjectHandle provider;
		if (variant.tryCast( provider ))
		{
			source = provider.getBase< ITreeModel >();
		}
	}

	impl_->filteredModel_.setSource( source );
}

QObject * WGFilteredTreeModel::getFilter() const
{
	return impl_->filter_;
}

void WGFilteredTreeModel::setFilter( QObject * filter )
{
	auto wgFilter = qobject_cast< WGFilter * >( filter );
	impl_->setFilter( wgFilter );
}

