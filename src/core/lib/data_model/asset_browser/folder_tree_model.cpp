#include "folder_tree_model.hpp"
#include "folder_tree_item.hpp"
#include "serialization/interfaces/i_file_system.hpp"
#include "logging/logging.hpp"
#include <list>

struct FolderTreeModel::Implementation
{
	Implementation( FolderTreeModel& main );
	~Implementation();

	std::vector<FolderTreeItem*> getSection( const FolderTreeItem* parent );
	void generateData( const FolderTreeItem* parent, const std::string& path );
	void clearData();

	FolderTreeModel& main_;
	IAssetBrowserModel* model_;
#ifdef _WIN32
	std::unordered_map<const FolderTreeItem*, std::vector<FolderTreeItem*>> data_;
#elif __APPLE__
	std::map<const FolderTreeItem*, std::vector<FolderTreeItem*>> data_;
#endif
	IFileSystem* fileSystem_;
};

FolderTreeModel::Implementation::Implementation( FolderTreeModel& main )
	: main_( main )
	, model_( nullptr )
	, fileSystem_( nullptr )
{
}

FolderTreeModel::Implementation::~Implementation()
{
	clearData();
}

void FolderTreeModel::Implementation::clearData()
{
	for (auto itr = data_.begin(); itr != data_.end(); ++itr)
	{
		auto items = itr->second;
		size_t max = items.size();

		for (size_t i = 0; i < max; ++i)
		{
			delete items[i];
		}
	}

	data_.clear();
}

std::vector<FolderTreeItem*> FolderTreeModel::Implementation::getSection(
	const FolderTreeItem* parent )
{
	auto itr = data_.lower_bound( parent );

	if (itr == data_.end())
	{
		return std::vector<FolderTreeItem*>();
	}
	return itr->second;
}

void FolderTreeModel::Implementation::generateData(
	const FolderTreeItem* parent, const std::string& path )
{
	std::list< std::pair< FolderTreeItem *, std::string> > folders;

	folders.push_back( std::make_pair( nullptr, path ) );

	while (!folders.empty())
	{
		FolderTreeItem* item = folders.front().first;
		const std::string& itemPath = folders.front().second;

		fileSystem_->enumerate( itemPath.c_str(), [&](FileInfo&& info) {
			if (info.isDots() || info.isHidden())
			{
				return true;
			}

			FolderTreeItem* childItem = new FolderTreeItem( info, item );
			data_[item].push_back( childItem );

			if (info.isDirectory())
			{
				folders.push_back(
					std::make_pair( childItem, info.fullPath ) );
			}
			return true;
		});

		folders.pop_front();
	}
}

FolderTreeModel::FolderTreeModel()
	: impl_( new Implementation( *this ) )
{
}

FolderTreeModel::FolderTreeModel( IAssetBrowserModel & model )
	: impl_( new Implementation( *this ) )
{
		init( &model );
}

FolderTreeModel::FolderTreeModel( const FolderTreeModel& rhs )
	: impl_( new Implementation( *this ) )

{
	init(rhs.model());
}

void FolderTreeModel::init( IAssetBrowserModel* model )
{
	impl_->model_ = model;
	impl_->fileSystem_ = model->fileSystem();

	setAssetPaths( model->assetPaths() );
}

IAssetBrowserModel* FolderTreeModel::model() const
{
	return impl_->model_;
}


void FolderTreeModel::setAssetPaths(const std::vector<std::string>& paths)
{
	impl_->clearData();
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
		impl_.reset( new Implementation( *this ) );
	}

	return *this;
}

IItem* FolderTreeModel::item( size_t index, const IItem* parent ) const
{
	auto temp = static_cast<const FolderTreeItem*>( parent );
	return impl_->getSection( temp )[index];
}

ITreeModel::ItemIndex FolderTreeModel::index( const IItem* item ) const
{
	auto temp = static_cast<const FolderTreeItem*>( item );
	temp = static_cast<const FolderTreeItem*>( temp->getParent() );
	ItemIndex index( 0, temp );

	auto items = impl_->getSection( temp );
	auto itr = std::find( items.begin(), items.end(), item );
	assert( itr != items.end() );

	index.first = itr - items.begin();
	return index;
}


bool FolderTreeModel::empty( const IItem* parent ) const
{
	const auto temp = static_cast< const FolderTreeItem* >( parent );
	return impl_->getSection( temp ).empty();
}


size_t FolderTreeModel::size( const IItem* parent ) const
{
	auto temp = static_cast<const FolderTreeItem*>( parent );
	return impl_->getSection( temp ).size();
}
