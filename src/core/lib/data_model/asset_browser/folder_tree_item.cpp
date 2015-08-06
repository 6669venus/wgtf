#include "folder_tree_item.hpp"
#include "data_model/generic_list.hpp"
#include "data_model/i_item_role.hpp"
#include "file_object_model.hpp"

struct FolderTreeItem::Implementation
{
	typedef std::vector<FolderTreeItem> FolderTreeItems;

	Implementation( FolderTreeItem& main, const FileInfo& fileInfo, const IItem* parent, IFileSystem& fileSystem );
	~Implementation();

	FolderTreeItems& getChildren() const;

	std::string				display_;
	FolderTreeItem&			main_;
	FileObjectModel			content_;
	IFileSystem&			fileSystem_;
	mutable FolderTreeItems	children_;
	
	const IItem* parent_;
};

FolderTreeItem::Implementation::Implementation(
	FolderTreeItem& main, const FileInfo& fileInfo, const IItem* parent, IFileSystem& fileSystem )
	: main_( main )
	, content_( fileInfo )
	, parent_( parent )
	, fileSystem_(fileSystem)
{
	display_ = fileInfo.fullPath;
	auto lastSeparator = display_.find_last_of("/\\");
	if (display_.length() > 1 && lastSeparator != std::string::npos)
	{
		if (lastSeparator == (display_.length() - 1))
			display_.erase(--display_.end());
		display_ = display_.substr(display_.find_last_of("/\\") + 1);
	}
}

FolderTreeItem::Implementation::~Implementation()
{
}

FolderTreeItem::Implementation::FolderTreeItems& FolderTreeItem::Implementation::getChildren() const
{
	// Not currently thread safe, only one thread can initialize the children
	const auto& info = content_.getFileInfo();
	if (info.isDirectory() && children_.empty())
	{
		fileSystem_.enumerate(info.fullPath.c_str(), [&](FileInfo&& info)
		{
			if (info.isDirectory() && !info.isDots() && !(info.attributes & FileAttributes::Hidden))
				children_.emplace_back(info, &main_, fileSystem_);
			return true;
		});
	}
	return children_;
}


FolderTreeItem::FolderTreeItem( const FileInfo& fileInfo, const IItem* parent, IFileSystem& fileSystem )
	: impl_( new Implementation( *this, fileInfo, parent, fileSystem ) )
{
}

FolderTreeItem::FolderTreeItem( const FolderTreeItem& rhs )
	: impl_( new Implementation( *this, rhs.impl_->content_.getFileInfo(), rhs.impl_->parent_, rhs.impl_->fileSystem_ ) )
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
			*this, rhs.impl_->content_.getFileInfo(), rhs.impl_->parent_, rhs.impl_->fileSystem_ ) );
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
	return impl_->display_.c_str();
}

ThumbnailData FolderTreeItem::getThumbnail( int column ) const
{
	return nullptr;
}

Variant FolderTreeItem::getData( int column, size_t roleId ) const
{
	if (column == 0 && roleId == ValueRole::roleId_)
	{
		return static_cast<IAssetObjectModel&>(impl_->content_);
	}

	return Variant();
}

bool FolderTreeItem::setData( int column, size_t roleId, const Variant& data )
{
	return false;
}

IItem* FolderTreeItem::operator[](size_t index) const
{
	return &impl_->getChildren()[index];
}

size_t FolderTreeItem::indexOf(const IItem* item) const
{
	auto& children = impl_->getChildren();
	return static_cast<const FolderTreeItem*>(item)-&*begin(impl_->getChildren());
}

bool FolderTreeItem::empty() const
{
	return impl_->getChildren().empty();
}

size_t FolderTreeItem::size() const
{
	return impl_->getChildren().size();
}