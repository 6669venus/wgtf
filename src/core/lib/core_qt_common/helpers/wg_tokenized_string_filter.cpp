#include "wg_tokenized_string_filter.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/filtering/tokenized_string_filter.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include <QString>
#include <QVariant>

struct WGTokenizedStringFilter::Implementation
{
	Implementation( WGTokenizedStringFilter & self );
	~Implementation();

	void setFilterText( const QString & filterText );
	void setSplitter( const QString & splitter );

	WGTokenizedStringFilter & self_;
	TokenizedStringFilter * filter_;
};

WGTokenizedStringFilter::Implementation::Implementation( WGTokenizedStringFilter & self )
	: self_( self )
	, filter_( new TokenizedStringFilter() )
{
}

WGTokenizedStringFilter::Implementation::~Implementation()
{
	if (filter_ != nullptr)
	{
		delete filter_;
		filter_ = nullptr;
	}
}

void WGTokenizedStringFilter::Implementation::setFilterText( const QString & filterText )
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

	filter_->updateFilterTokens( inputValue.c_str() );
	filter_->notifyFilterChanged();
}

void WGTokenizedStringFilter::Implementation::setSplitter( const QString & splitter )
{
	if (filter_ == nullptr)
	{
		return;
	}

	std::string inputValue = splitter.toUtf8().constData();
	if (strcmp( inputValue.c_str(), filter_->getSplitterChar() ) == 0)
	{
		return;
	}

	filter_->setSplitterChar( inputValue.c_str() );
}

WGTokenizedStringFilter::WGTokenizedStringFilter()
	: impl_( new Implementation( *this ) )
{
}

WGTokenizedStringFilter::~WGTokenizedStringFilter()
{
}

QString WGTokenizedStringFilter::getFilterText() const
{
	if (impl_->filter_ != nullptr)
	{
		return QString::fromStdString( impl_->filter_->getFilterText() );
	}
	
	return "";
}

void WGTokenizedStringFilter::setFilterText( const QString & filterText )
{
	impl_->setFilterText( filterText );
}

IItemFilter * WGTokenizedStringFilter::getFilter() const
{
	return static_cast< IItemFilter * >( impl_->filter_ );
}

QString WGTokenizedStringFilter::getSplitterChar() const
{
	if (impl_->filter_ != nullptr)
	{
		return QString::fromStdString( impl_->filter_->getSplitterChar() );
	}

	return "";
}

void WGTokenizedStringFilter::setSplitterChar( const QString & splitter )
{
	impl_->setSplitter( splitter );
	emit splitterCharChanged();
}

