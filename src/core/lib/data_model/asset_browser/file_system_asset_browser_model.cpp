
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_model.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "file_system_asset_browser_model.hpp"

#include "data_model/asset_browser/folder_content_object_model.hpp"
#include "data_model/asset_browser/folder_tree_item.hpp"
#include "data_model/asset_browser/folder_tree_model.hpp"
#include "data_model/generic_list.hpp"
#include "data_model/i_item_role.hpp"
#include "data_model/i_tree_model.hpp"
#include "data_model/value_change_notifier.hpp"
#include "generic_plugin/interfaces/i_context_manager.hpp"
#include "logging/logging.hpp"
#include "serialization/interfaces/i_file_system.hpp"

#include "asset_browser/i_asset_listener.hpp"

static const size_t NO_SELECTION = SIZE_MAX;

struct FileSystemAssetBrowserModel::FileSystemAssetBrowserModelImplementation
{
	FileSystemAssetBrowserModel::FileSystemAssetBrowserModelImplementation(
		FileSystemAssetBrowserModel& self,
		const AssetPaths& paths,
		IFileSystem& fileSystem,
		IDefinitionManager& definitionManager )
		: self_( self )
		, fileSystem_( fileSystem )
		, currentSelectedAssetIndex_( -1 )
		, currentBreadcrumbItemIndex_( NO_SELECTION )
		, definitionManager_( definitionManager )
		, folders_( nullptr )
	{
		generateBreadcrumbs();
	}

	void addBreadcrumb(const char* value)
	{
		breadcrumbs_.push_back(value);
	}

	void addFolderItem(const FileInfo& fileInfo)
	{
		ObjectHandle object =
			definitionManager_.create<FolderContentObjectModel>(false);

		object.getBase<FolderContentObjectModel>()->init(fileInfo);

		folderContents_.push_back(object);
	}

	void generateBreadcrumbs()
	{
		breadcrumbs_.clear();
	}

	FileSystemAssetBrowserModel& self_;
	GenericList	folderContents_;
	GenericList	breadcrumbs_;
	int			currentSelectedAssetIndex_;
	size_t		currentBreadcrumbIndex_;

	std::vector<IAssetListener*>	listeners_;
	std::shared_ptr<ITreeModel>		folders_;
	std::vector<ITreeModel::ItemIndex> foldersCrumb_;
	ValueChangeNotifier< size_t > currentBreadcrumbItemIndex_;
	std::vector<size_t> folderItemIndexHistory_;

	IDefinitionManager&	definitionManager_;
	IFileSystem&		fileSystem_;
	AssetPaths			assetPaths_;
};

FileSystemAssetBrowserModel::FileSystemAssetBrowserModel(
	const AssetPaths& assetPaths, IFileSystem& fileSystem, IDefinitionManager& definitionManager )
	: impl_(new FileSystemAssetBrowserModelImplementation( *this, assetPaths, fileSystem, definitionManager ) )
{
	for (auto& path : assetPaths)
	{
		addAssetPath(path);
	}
	// Create the FolderTreeModel now that we've added our asset paths
	impl_->folders_.reset(new FolderTreeModel(*this, impl_->fileSystem_));
}

void FileSystemAssetBrowserModel::addAssetPath(const std::string& path)
{
	if (std::find(impl_->assetPaths_.begin(), impl_->assetPaths_.end(), path)
		== impl_->assetPaths_.end())
	{
		if (!impl_->fileSystem_.exists(path.c_str()))
		{
			NGT_ERROR_MSG("TestAssetBrowserModel::addAssetPath: "
				"asset folder path does not exist: %s\n", path.c_str());
			return;
		}

		impl_->assetPaths_.push_back(path);
	}
}

void FileSystemAssetBrowserModel::initialise( IContextManager& contextManager )
{
}

void FileSystemAssetBrowserModel::addListener( IAssetListener* listener )
{
	if (listener == nullptr)
	{
		return;
	}

	impl_->listeners_.push_back(listener);
}

const AssetPaths& FileSystemAssetBrowserModel::assetPaths() const
{
	return impl_->assetPaths_;
}

void FileSystemAssetBrowserModel::populateFolderContents( const AssetPaths& paths )
{
	impl_->folderContents_.clear();
	addFolderItems(paths);
}

ObjectHandle FileSystemAssetBrowserModel::getBreadcrumbs() const
{
	return impl_->breadcrumbs_;
}

ObjectHandle FileSystemAssetBrowserModel::getFolderContents() const
{
	return impl_->folderContents_;
}

ObjectHandle FileSystemAssetBrowserModel::getFolderTreeModel() const
{
	return impl_->folders_.get();
}

bool FileSystemAssetBrowserModel::navigateHistoryForward() const
{
	// Update the current breadcrumb item index and let the listeners know
	// the data has been changed.
	if (impl_->foldersCrumb_.size() > impl_->currentBreadcrumbIndex_ + 1)
	{
		impl_->currentBreadcrumbIndex_ += 1;
		impl_->currentBreadcrumbItemIndex_.value(impl_->currentBreadcrumbIndex_);
	}

	return true;
}

bool FileSystemAssetBrowserModel::navigateHistoryBackward() const
{
	// Update the current breadcrumb item index and let the listeners know
	// the data has been changed.
	if (0 < impl_->currentBreadcrumbIndex_)
	{
		impl_->currentBreadcrumbIndex_ -= 1;
		impl_->currentBreadcrumbItemIndex_.value(impl_->currentBreadcrumbIndex_);
	}

	return true;
}

Variant FileSystemAssetBrowserModel::getFolderTreeItemSelected() const
{
	return Variant();
}

void FileSystemAssetBrowserModel::setFolderTreeItemSelected(const Variant& selectedItem)
{
	auto item = reinterpret_cast< const FolderTreeItem *>(selectedItem.value<intptr_t>());

	if (item)
	{
		const FileInfo& fileInfo = item->getFileInfo();
		std::vector< std::string > paths;
		paths.push_back(fileInfo.fullPath);

		std::string token = "\\";
		auto lastToken = std::find_end(fileInfo.fullPath.begin(), fileInfo.fullPath.end(), token.begin(), token.end());

		if (impl_->folders_)
		{
			ITreeModel::ItemIndex selectedItemIndex = impl_->folders_->index(item);
			auto foundItemIndex = std::find(impl_->foldersCrumb_.begin(), impl_->foldersCrumb_.end(), selectedItemIndex);

			// Don't add same ItemIndex twice
			if (impl_->foldersCrumb_.end() == foundItemIndex)
			{
				// Keep the folder item index history and update current breadcrumb index
				impl_->folderItemIndexHistory_.push_back(selectedItemIndex.first);
				impl_->currentBreadcrumbIndex_ = (impl_->folderItemIndexHistory_.size() - 1);

				impl_->foldersCrumb_.push_back(selectedItemIndex);

				if (fileInfo.fullPath.end() != lastToken)
				{
					// Just grab the token and the folder name.
					// e.g. "\models"
					std::string crumb = "";
					crumb.append(lastToken, fileInfo.fullPath.end());
					impl_->addBreadcrumb(crumb.c_str());
				}
			}
		}

		this->populateFolderContents(paths);
	}
}

size_t FileSystemAssetBrowserModel::getFolderTreeItemIndex() const
{
	if (impl_->folderItemIndexHistory_.size() <= 0 ||
		NO_SELECTION == impl_->currentBreadcrumbIndex_)
	{
		return 0;
	}

	return impl_->folderItemIndexHistory_[impl_->currentBreadcrumbIndex_];
}

ObjectHandle FileSystemAssetBrowserModel::currentBreadcrumbItemIndex() const
{
	return ObjectHandle(&impl_->currentBreadcrumbItemIndex_);
}

const size_t & FileSystemAssetBrowserModel::getCurrentBreadcrumbItemIndex() const
{
	return impl_->currentBreadcrumbIndex_;
}

void FileSystemAssetBrowserModel::setCurrentBreadcrumbItemIndex(const size_t & index)
{
	impl_->currentBreadcrumbIndex_ = index;
	impl_->currentBreadcrumbItemIndex_.value(index);
}

bool FileSystemAssetBrowserModel::useSelectedAsset() const
{
	if (impl_->currentSelectedAssetIndex_ > -1)
	{
		auto folderContents = getFolderContents().getBase<GenericList>();
		assert(folderContents != nullptr);
		auto item = folderContents->item(currentSelectedAssetIndex());
		assert(item != NULL);
		auto& model = item->getData(0, ValueRole::roleId_).castRef<FolderContentObjectModel>();
		for (auto listener : impl_->listeners_)
		{
			if (listener != nullptr)
			{
				listener->useAsset(model.getFullPath());
			}
		}
	}

	return true;
}

const int & FileSystemAssetBrowserModel::currentSelectedAssetIndex() const
{
	return impl_->currentSelectedAssetIndex_;
}

void FileSystemAssetBrowserModel::currentSelectedAssetIndex(const int & index)
{
	impl_->currentSelectedAssetIndex_ = index;
}

void FileSystemAssetBrowserModel::addFolderItems( const AssetPaths& paths )
{
	IFileSystem& fs = impl_->fileSystem_;

	std::list<std::string> directories;

	for (auto& path : paths)
	{
		if (!fs.exists(path.c_str()))
		{
			NGT_WARNING_MSG("TestAssetBrowserModel::addFolderItems: "
				"asset folder path does not exist: %s\n", path.c_str());
			continue;
		}

		directories.push_back(path);
	}

	while (!directories.empty())
	{
		const std::string& dir = directories.front();

		fs.enumerate(dir.c_str(), [&](FileInfo&& info) {
			if (!info.isDirectory())
			{
				impl_->addFolderItem(info);
			}
			// TODO: For search/filtering we should add all resources on a separate thread
			// We don't want to block the main UI thread
			// For now do not add sub-folders to avoid performance issues
			//else if (!info.isDots())
			//{
			//	directories.push_back( info.fullPath );
			//}
			return true;
		});

		directories.pop_front();
	}
}