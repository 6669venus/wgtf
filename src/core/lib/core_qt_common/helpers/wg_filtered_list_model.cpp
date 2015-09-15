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

	void setFilter( IItemFilter * filter );

	WGFilteredListModel & self_;
	IItemFilter * filter_;
	std::shared_ptr< FilteredListModel > filteredSource_;
};

WGFilteredListModel::Implementation::Implementation( WGFilteredListModel & self )
	: self_( self )
{
}

void WGFilteredListModel::Implementation::setFilter( IItemFilter * filter )
{
	if (filter_ == filter)
	{
		return;
	}

	filter_ = filter;

	filteredSource_ = nullptr;
	if (self_.source() != nullptr)
	{
		filteredSource_ = std::unique_ptr< FilteredListModel >( new FilteredListModel( *self_.source(), *filter ) );
	}

	emit self_.filterChanged();
}

WGFilteredListModel::WGFilteredListModel()
	: impl_( new Implementation( *this ) )
{
}

WGFilteredListModel::~WGFilteredListModel()
{
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
