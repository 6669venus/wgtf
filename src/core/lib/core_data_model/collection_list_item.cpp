#include "collection_list_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include <cassert>

class CollectionListItem::Impl
{
public:
	Impl( const char* displayText,
		const ThumbnailData & thumbnail,
		const Variant* data,
		size_t dataLength );

	const std::string displayText_;
	const ThumbnailData thumbnail_;
	const std::vector< Variant > items_;
};


CollectionListItem::Impl::Impl( const char* displayText,
	const ThumbnailData & thumbnail,
	const Variant* data,
	size_t dataLength )
	: displayText_( displayText != nullptr ? displayText : "" )
	, thumbnail_( thumbnail )
	, items_( data, (data + dataLength) )
{
}


CollectionListItem::CollectionListItem( const char* displayText,
	const ThumbnailData & thumbnail,
	const Variant* data,
	size_t dataLength )
	: IItem()
	, impl_( new Impl( displayText, thumbnail, data, dataLength ) )
{}


int CollectionListItem::columnCount() const
{
	return static_cast< int >( impl_->items_.size() );
}


const char* CollectionListItem::getDisplayText( int column ) const
{
	return impl_->displayText_.c_str();
}


ThumbnailData CollectionListItem::getThumbnail( int column ) const
{
	return impl_->thumbnail_;
}


Variant CollectionListItem::getData( int column, size_t roleId ) const
{
	if (column >= this->columnCount())
	{
		return Variant();
	}

	if (roleId == ValueRole::roleId_)
	{
		return impl_->items_.at( column );
	}
	else if (roleId == IndexPathRole::roleId_)
	{
		return std::string( "" );
	}

	return Variant();
}


bool CollectionListItem::setData( int column,
	size_t roleId,
	const Variant & data )
{
	assert( false && "Not implemented" );
	return false; 
}

