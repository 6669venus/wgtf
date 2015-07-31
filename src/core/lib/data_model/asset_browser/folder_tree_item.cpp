#include "folder_tree_item.hpp"
#include "data_model/generic_list.hpp"
#include "data_model/i_item_role.hpp"
#include "folder_content_object_model.hpp"

struct FolderTreeItem::Implementation
{
	Implementation( FolderTreeItem& main,
		const FolderContentObjectModel& content, const IItem* parent );

	Implementation( FolderTreeItem& main,
		const FileInfo& fileInfo, const IItem* parent );
	~Implementation();

	FolderTreeItem& main_;
	FolderContentObjectModel content_;
	
	const IItem* parent_;
};

FolderTreeItem::Implementation::Implementation(
	FolderTreeItem& main, const FileInfo& fileInfo, const IItem* parent )
	: main_( main )
	, content_( fileInfo )
	, parent_( parent )
{
}

FolderTreeItem::Implementation::Implementation(
	FolderTreeItem& main, const FolderContentObjectModel& content,
	const IItem* parent )
	: main_( main )
	, content_( content )
	, parent_( parent )
{
}

FolderTreeItem::Implementation::~Implementation()
{
}


FolderTreeItem::FolderTreeItem( const FileInfo& fileInfo, const IItem* parent )
	: impl_( new Implementation( *this, fileInfo, parent ) )
{
}

FolderTreeItem::FolderTreeItem( const FolderTreeItem& rhs )
	: impl_( new Implementation( *this, rhs.impl_->content_, rhs.impl_->parent_ ) )
{
}

FolderTreeItem::~FolderTreeItem()
{
}

FolderTreeItem& FolderTreeItem::operator=( const FolderTreeItem& rhs )
{
	if (this != &rhs)
	{
		impl_.reset( new Implementation(
			*this, rhs.impl_->content_, rhs.impl_->parent_ ) );
	}

	return *this;
}

const IItem* FolderTreeItem::getParent() const
{
	return impl_->parent_;
}

const FileInfo& FolderTreeItem::getFileInfo() const
{
	return impl_->content_.getFileInfo();
}

int FolderTreeItem::columnCount() const
{
	return 1;
}

const char* FolderTreeItem::getDisplayText( int column ) const
{
	return impl_->content_.getFileName();
}

ThumbnailData FolderTreeItem::getThumbnail( int column ) const
{
	return nullptr;
}

Variant FolderTreeItem::getData( int column, size_t roleId ) const
{
	if (column == 0 && roleId == ValueRole::roleId_)
	{
		return impl_->content_;
	}

	return Variant();
}

bool FolderTreeItem::setData( int column, size_t roleId, const Variant& data )
{
	return false;
}
