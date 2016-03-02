#include "file_system_model.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_serialization/interfaces/i_file_system.hpp"

#include <string>

namespace
{
	class FileItem;
	typedef std::vector< std::unique_ptr< FileItem > > FileItems;

	class FileItem : public IItem
	{
	public:
		FileItem( FileInfo && fileInfo, const FileItem * parent )
			: fileInfo_( fileInfo )
			, parent_( parent )
		{

		}

		virtual const char * getDisplayText(int column) const
		{
			return fileInfo_.name();
		}

		virtual ThumbnailData getThumbnail(int column) const
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

		virtual Variant getData(int column, size_t roleId) const
		{
			if (roleId == IndexPathRole::roleId_)
			{
				return fileInfo_.fullPath;
			}

			return Variant();
		}

		virtual bool setData(int column, size_t roleId, const Variant & data)
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

		const FileInfo fileInfo_;
		const FileItem * parent_;
		const FileItems children_;
	};
}

struct FileSystemModel::Impl 
{
	Impl( IFileSystem & fileSystem, const char * rootDirectory )
		: fileSystem_( fileSystem )
		, rootDirectory_( rootDirectory )
	{
	}

	IFileSystem & fileSystem_;
	std::string rootDirectory_;
	FileItems rootItems_;
};

FileSystemModel::FileSystemModel( IFileSystem & fileSystem, const char * rootDirectory )
	: impl_( new Impl( fileSystem, rootDirectory ) )
{
}

FileSystemModel::~FileSystemModel()
{
}

IItem * FileSystemModel::item(size_t index, const IItem * parent) const
{
	auto parentItem = static_cast< const FileItem * >( parent );

	// See if we have cached this item already
	auto & fileItems = parentItem != nullptr ? parentItem->children_ : impl_->rootItems_;
	if (index < fileItems.size() )
	{
		auto fileItem = fileItems[index].get();
		if (fileItem != nullptr)
		{
			return fileItem;
		}
	}

	// Item not cached, must enumerate
	auto & directory = parentItem != nullptr ? parentItem->fileInfo_.fullPath : impl_->rootDirectory_;
	size_t i = 0;
	impl_->fileSystem_.enumerate( directory.c_str(), [&]( FileInfo && fileInfo )
	{
		// Skip dots and hidden files
		if (fileInfo.isDots() || fileInfo.isHidden())
		{
			return true;
		}

		if (i == fileItems.size())
		{
			std::unique_ptr< FileItem > fileItem( new FileItem( std::move( fileInfo ), parentItem ) );
			const_cast< FileItems & >( fileItems ).emplace_back( std::move( fileItem ) );
			if (i == index)
			{
				return false;
			}
		}
		++i;
		return true;
	} );
	return fileItems.back().get();
}

ITreeModel::ItemIndex FileSystemModel::index(const IItem * item) const
{
	assert( item != nullptr );
	auto fileItem = static_cast< const FileItem * >( item );
	auto parentItem = fileItem->parent_;

	// See if we have cached this item already
	auto & fileItems = parentItem != nullptr ? parentItem->children_ : impl_->rootItems_;
	auto findIt = std::find_if( fileItems.begin(), fileItems.end(), [&]( const std::unique_ptr< FileItem > & value )
	{
		return value.get() == fileItem;
	} );
	if (findIt != fileItems.end())
	{
		return ItemIndex( std::distance( fileItems.begin(), findIt ), parentItem );
	}

	// Item not cached, must enumerate
	auto & directory = parentItem != nullptr ? parentItem->fileInfo_.fullPath : impl_->rootDirectory_;
	size_t i = 0;
	impl_->fileSystem_.enumerate( directory.c_str(), [&]( FileInfo && fileInfo )
	{
		// Skip dots and hidden files
		if (fileInfo.isDots() || fileInfo.isHidden())
		{
			return true;
		}

		if (strcmp( fileItem->fileInfo_.name(), fileInfo.name() ) == 0)
		{
			return false;
		}
		++i;
		return true;
	} );
	return ItemIndex( i, parentItem );
}

bool FileSystemModel::empty(const IItem * item) const
{
	auto fileItem = static_cast< const FileItem * >( item );

	// See if we have cached this item already
	auto & fileItems = fileItem != nullptr ? fileItem->children_ : impl_->rootItems_;
	if (!fileItems.empty())
	{
		return false;
	}

	// Item not cached, must enumerate
	auto & directory = fileItem != nullptr ? fileItem->fileInfo_.fullPath : impl_->rootDirectory_;
	bool empty = true;
	impl_->fileSystem_.enumerate( directory.c_str(), [&]( FileInfo && fileInfo )
	{
		// Skip dots and hidden files
		if (fileInfo.isDots() || fileInfo.isHidden())
		{
			return true;
		}

		empty = false;
		return false;
	} );
	return empty;
}

size_t FileSystemModel::size(const IItem * item) const
{
	auto fileItem = static_cast< const FileItem * >( item );

	auto & directory = fileItem != nullptr ? fileItem->fileInfo_.fullPath : impl_->rootDirectory_;
	size_t size = 0;
	impl_->fileSystem_.enumerate( directory.c_str(), [&]( FileInfo && fileInfo )
	{
		// Skip dots and hidden files
		if (fileInfo.isDots() || fileInfo.isHidden())
		{
			return true;
		}

		++size;
		return true;
	} );
	return size;
}

int FileSystemModel::columnCount() const
{
	return 1;
}

