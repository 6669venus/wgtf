#include "folder_tree_model.hpp"
#include "folder_tree_item.hpp"
#include "serialization/interfaces/i_file_system.hpp"
#include "logging/logging.hpp"

#include <vector>

typedef std::unique_ptr<IItem> IItem_uptr;

struct FolderTreeModel::Implementation
{
	Implementation( FolderTreeModel& main, IFileSystem& fileSystem );
	~Implementation();

	std::vector<FolderTreeItem*> getSection( const FolderTreeItem* parent );
	void generateData( const FolderTreeItem* parent, const std::string& path );

	FolderTreeModel&		main_;
	IFileSystem&			fileSystem_;
	IAssetBrowserModel*		model_;
	std::vector<IItem_uptr>	roots_;
};

FolderTreeModel::Implementation::Implementation( FolderTreeModel& main, IFileSystem& fileSystem )
	: main_( main )
	, model_( nullptr )
	, fileSystem_( fileSystem )
{
}

FolderTreeModel::Implementation::~Implementation()
{
}

void FolderTreeModel::Implementation::generateData(
	const FolderTreeItem* parent, const std::string& path )
{
	auto info = fileSystem_.getFileInfo(path.c_str());
	if ((info.attributes != FileAttributes::None) && !info.isDots() && !info.isHidden())
	{
		roots_.emplace_back(new FolderTreeItem(info, nullptr, fileSystem_));
	}
}

FolderTreeModel::FolderTreeModel( IAssetBrowserModel & model, IFileSystem& fileSystem )
	: impl_( new Implementation( *this, fileSystem ) )
{
	init( &model );
}

FolderTreeModel::FolderTreeModel( const FolderTreeModel& rhs )
	: impl_( new Implementation( *this, rhs.impl_->fileSystem_ ) )

{
	init(rhs.model());
}

void FolderTreeModel::init( IAssetBrowserModel* model )
{
	impl_->model_ = model;

	setAssetPaths( model->assetPaths() );
}

IAssetBrowserModel* FolderTreeModel::model() const
{
	return impl_->model_;
}


void FolderTreeModel::setAssetPaths(const std::vector<std::string>& paths)
{
	for (auto& path : paths)
	{
		impl_->generateData( nullptr, path );
	}

	impl_->model_->populateFolderContents( paths );
}

FolderTreeModel::~FolderTreeModel()
{
}

FolderTreeModel& FolderTreeModel::operator=( const FolderTreeModel& rhs )
{
	if (this != &rhs)
	{
		impl_.reset( new Implementation( *this, rhs.impl_->fileSystem_ ) );
	}

	return *this;
}

IItem* FolderTreeModel::item( size_t index, const IItem* parent ) const
{
	auto temp = static_cast<const FolderTreeItem*>( parent );
	return temp ? (*temp)[index] : const_cast<IItem*>(impl_->roots_[index].get());
}

ITreeModel::ItemIndex FolderTreeModel::index( const IItem* item ) const
{
	auto temp = static_cast<const FolderTreeItem*>(item);
	temp = static_cast<const FolderTreeItem*>(temp->getParent());
	if (temp)
		return ItemIndex(temp->indexOf(item), temp);

	auto found = std::find_if(impl_->roots_.begin(), impl_->roots_.end(),
		[&](const IItem_uptr& i){ return i.get() == item; });

	return ItemIndex(found - impl_->roots_.begin(), temp);
}


bool FolderTreeModel::empty( const IItem* parent ) const
{
	auto temp = static_cast< const FolderTreeItem* >( parent );
	return temp->empty();
}


size_t FolderTreeModel::size( const IItem* parent ) const
{
	auto temp = static_cast<const FolderTreeItem*>( parent );
	return temp ? temp->size() : impl_->roots_.size();
}
