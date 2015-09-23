#include "file_object_item.hpp"
#include "i_asset_object_model.hpp"
#include "core_data_model/i_item_role.hpp"

struct FileObjectItem::Implementation
{
	Implementation( FileObjectItem & self, IAssetObjectModel & model )
		: self_( self )
		, model_( model )
	{
	}

	FileObjectItem & self_;
	IAssetObjectModel & model_;
};

FileObjectItem::FileObjectItem( IAssetObjectModel & model )
	: impl_( new Implementation( *this, model ) )
{
}

FileObjectItem::~FileObjectItem()
{
}

int FileObjectItem::columnCount() const
{
	return 1;
}

const char * FileObjectItem::getDisplayText( int column ) const
{
	return impl_->model_.getFileName();
}

ThumbnailData FileObjectItem::getThumbnail( int column ) const
{
	return nullptr;
}

Variant FileObjectItem::getData( int column, size_t roleId ) const
{
	if (column == 0 && roleId == ValueRole::roleId_)
	{
		return ObjectHandleT<IAssetObjectModel>(&impl_->model_);
	}

	return Variant();
}

bool FileObjectItem::setData( int column, size_t roleId, const Variant & data )
{
	return false;
}