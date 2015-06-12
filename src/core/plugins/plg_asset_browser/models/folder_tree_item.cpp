#include "folder_tree_item.hpp"
#include "data_model/generic_list.hpp"
#include "data_model/i_item_role.hpp"

struct FolderTreeItem::Implementation
{
	Implementation( FolderTreeItem& main, const char* name, const IItem* parent );
	~Implementation();

	FolderTreeItem& main_;
	const char* name_;
	std::string lastModified_;
	std::string folderType_;
	int size_;
	const IItem* parent_;
};

FolderTreeItem::Implementation::Implementation(
	FolderTreeItem& main, const char* name, const IItem* parent )
	: main_( main )
	, name_( name )
	, lastModified_( "5/20/2015 1:23 PM" )
	, folderType_( "File Folder" )
	, size_( 123 )
	, parent_( parent )
{
}

FolderTreeItem::Implementation::~Implementation()
{
	delete name_;
}


FolderTreeItem::FolderTreeItem( const char* name, const IItem* parent )
	: impl_( new Implementation( *this, name, parent ) )
{
}

FolderTreeItem::FolderTreeItem( const FolderTreeItem& rhs )
	: impl_( new Implementation( *this, rhs.impl_->name_, rhs.impl_->parent_ ) )
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
			*this, rhs.impl_->name_, rhs.impl_->parent_ ) );
	}

	return *this;
}

const IItem* FolderTreeItem::getParent() const
{
	return impl_->parent_;
}

int FolderTreeItem::columnCount() const
{
	return 1;
}

const char* FolderTreeItem::getDisplayText( int column ) const
{
	return impl_->name_;
}

ThumbnailData FolderTreeItem::getThumbnail( int column ) const
{
	return nullptr;
}

Variant FolderTreeItem::getData( int column, size_t roleId ) const
{
	return Variant();
	/*TODO - this is all wrong. Assumptions made about this giving us column
		     names, but it's actually a way to tie roles with specific data in qml
	if (column > this->columnCount())
	{
		return Variant();
	}

	switch (column)
	{
	case 0:		
		{
			if (roleId == ValueRole::roleId_)
			{
				return impl_->name_;
			}
		}
		break;

	case 1:
		{
			if (roleId == ValueRole::roleId_)
			{
				return impl_->size_;
			}
		}
		break;

	case 2:
		{
			if (roleId == ValueRole::roleId_)
			{
				return impl_->folderType_.c_str();
			}
		}
		break;
		
	case 3:
		{
			if (roleId == ValueRole::roleId_)
			{
				return impl_->lastModified_.c_str();
			}
		}
		break;
	}

	return Variant();
	*/
}

bool FolderTreeItem::setData( int column, size_t roleId, const Variant& data )
{
	return false;
}
