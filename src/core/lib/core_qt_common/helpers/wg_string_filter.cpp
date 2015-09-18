#include "wg_string_filter.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/filtering/string_filter.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include <QString>
#include <QVariant>

struct WGStringFilter::Implementation
{
	Implementation( WGStringFilter & self );
	~Implementation();

	void setFilterText( const QString & filterText );

	WGStringFilter & self_;
	StringFilter * filter_;
};

WGStringFilter::Implementation::Implementation( WGStringFilter & self )
	: self_( self )
	, filter_( new StringFilter() )
{
}

WGStringFilter::Implementation::~Implementation()
{
	if (filter_ != nullptr)
	{
		delete filter_;
		filter_ = nullptr;
	}
}

void WGStringFilter::Implementation::setFilterText( const QString & filterText )
{
	if (filter_ == nullptr)
	{
		return;
	}

	std::string inputValue = filterText.toUtf8().constData();
	if (strcmp( inputValue.c_str(), filter_->getFilterText() ) == 0)
	{
		return;
	}

	filter_->setFilterText( inputValue.c_str() );
	filter_->notifyFilterChanged();
}

WGStringFilter::WGStringFilter()
	: impl_( new Implementation( *this ) )
{
}

WGStringFilter::~WGStringFilter()
{
}

QString WGStringFilter::getFilterText() const
{
	if (impl_->filter_ != nullptr)
	{
		return QString::fromStdString( impl_->filter_->getFilterText() );
	}

	return "";
}

void WGStringFilter::setFilterText( const QString & filterText )
{
	impl_->setFilterText( filterText );
}

IItemFilter * WGStringFilter::getFilter() const
{
	return static_cast< IItemFilter * >( impl_->filter_ );
}
