#include "asset_object_item.hpp"
#include "i_asset_object_model.hpp"
#include "core_data_model/i_item_role.hpp"

struct AssetObjectItem::Implementation
{
	Implementation( AssetObjectItem & self, const FileInfo & fileInfo )
		: self_( self )
		, fileInfo_( fileInfo )
	{
	}

	Implementation(	AssetObjectItem & self )
	: self_( self )
	{
	}

	AssetObjectItem & self_;
	FileInfo fileInfo_;
};

AssetObjectItem::AssetObjectItem()
	: impl_( new Implementation( *this ) )
{
}

AssetObjectItem::AssetObjectItem( const AssetObjectItem & rhs )
	: impl_( new Implementation( *this, rhs.impl_->fileInfo_ ) )
{
}

AssetObjectItem::AssetObjectItem( const FileInfo & fileInfo )
	: impl_( new Implementation( *this, fileInfo ) )
{
}

AssetObjectItem::~AssetObjectItem()
{
}

int AssetObjectItem::columnCount() const
{
	return 1;
}

const char * AssetObjectItem::getDisplayText( int column ) const
{
	return getFileName();
}

ThumbnailData AssetObjectItem::getThumbnail( int column ) const
{
	// TODO: Support thumbnails in the asset browser.
	// JIRA: http://jira.bigworldtech.com/browse/NGT-1107
	return nullptr;
}

Variant AssetObjectItem::getData( int column, size_t roleId ) const
{
	if (column != 0)
	{
		return Variant();
	}

	if (roleId == ValueRole::roleId_)
	{
		return getFileName();
	}
	else if (roleId == IndexPathRole::roleId_)
	{
		return getFullPath();
	}
	else if (roleId == ThumbnailRole::roleId_)
	{
		return getThumbnail( 0 );
	}
	else if (roleId == SizeRole::roleId_)
	{
		return getSize();
	}
	else if (roleId == CreatedTimeRole::roleId_)
	{
		return getCreatedTime();
	}
	else if (roleId == ModifiedTimeRole::roleId_)
	{
		return getModifiedTime();
	}
	else if (roleId == AccessedTimeRole::roleId_)
	{
		return getAccessedTime();
	}
	else if (roleId == IsDirectoryRole::roleId_)
	{
		return isDirectory();
	}
	else if (roleId == IsReadOnlyRole::roleId_)
	{
		return isReadOnly();
	}
	else if (roleId == IsCompressedRole::roleId_)
	{
		return isCompressed();
	}

	return Variant();
}

bool AssetObjectItem::setData( int column, size_t roleId, const Variant & data )
{
	return false;
}

const FileInfo& AssetObjectItem::getFileInfo() const
{
	return impl_->fileInfo_;
}

const char* AssetObjectItem::getFileName() const
{
	return impl_->fileInfo_.name();
}

const char* AssetObjectItem::getFullPath() const
{
	return impl_->fileInfo_.fullPath.c_str();
}

uint64_t AssetObjectItem::getSize() const
{
	return impl_->fileInfo_.size;
}

uint64_t AssetObjectItem::getCreatedTime() const
{
	return impl_->fileInfo_.created;
}

uint64_t AssetObjectItem::getModifiedTime() const
{
	return impl_->fileInfo_.modified;
}

uint64_t AssetObjectItem::getAccessedTime() const
{
	return impl_->fileInfo_.accessed;
}

bool AssetObjectItem::isDirectory() const
{
	return impl_->fileInfo_.isDirectory();
}

bool AssetObjectItem::isReadOnly() const
{
	return impl_->fileInfo_.isReadOnly();
}

bool AssetObjectItem::isCompressed() const
{
	return (impl_->fileInfo_.attributes & FileAttributes::Compressed) == FileAttributes::Compressed;
}