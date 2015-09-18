#include "wg_filter.hpp"
#include "core_data_model/filtering/i_item_filter.hpp"
#include <QVariant>

struct WGFilter::Implementation
{
	Implementation( WGFilter & self );

	WGFilter & self_;
};

WGFilter::Implementation::Implementation( WGFilter & self )
	: self_( self )
{
}

WGFilter::WGFilter()
	: impl_( new Implementation( *this ) )
{
}

WGFilter::~WGFilter()
{
}

IItemFilter * WGFilter::getFilter() const
{
	return nullptr;
}

