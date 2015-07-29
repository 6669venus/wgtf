
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_model.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "file_system_asset_browser_model.hpp"

#include "data_model/asset_browser/file_object_model.hpp"
#include "data_model/asset_browser/folder_tree_item.hpp"
#include "data_model/asset_browser/folder_tree_model.hpp"
#include "data_model/generic_list.hpp"
#include "data_model/i_item_role.hpp"
#include "data_model/i_tree_model.hpp"
#include "data_model/value_change_notifier.hpp"
#include "generic_plugin/interfaces/i_component_context.hpp"
#include "logging/logging.hpp"
#include "serialization/interfaces/i_file_system.hpp"
#include "reflection/type_class_definition.hpp"

struct FileSystemAssetBrowserModel::FileSystemAssetBrowserModelImplementation
{
	FileSystemAssetBrowserModel::FileSystemAssetBrowserModelImplementation(
		FileSystemAssetBrowserModel& self,
		const AssetPaths& paths,
		IFileSystem& fileSystem,
		IDefinitionManager& definitionManager )
		: self_( self )
		, fileSystem_( fileSystem )
		, definitionManager_( definitionManager )
		, folders_( nullptr )
	{
	}

	void addFolderItem(const FileInfo& fileInfo)
	{
		auto assetObjectDef = definitionManager_.getDefinition<IAssetObjectModel>();
		if(assetObjectDef)
		{
			auto object = TypeClassDefinition<FileObjectModel>::create(*assetObjectDef, fileInfo);
			folderContents_.push_back(object);
		}
	}

	FileSystemAssetBrowserModel& self_;
	GenericList	folderContents_;
	std::shared_ptr<ITreeModel>		folders_;

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

void FileSystemAssetBrowserModel::initialise( IComponentContext& contextManager )
{
}

const AssetPaths& FileSystemAssetBrowserModel::assetPaths() const
{
	return impl_->assetPaths_;
}

void FileSystemAssetBrowserModel::populateFolderContents( const IItem* item )
{
	impl_->folderContents_.clear();
	if ( item )
	{
		auto folderItem = static_cast<const FolderTreeItem *>( item );
		if ( folderItem )
		{
			std::vector< std::string > paths;
			const FileInfo& fileInfo = folderItem->getFileInfo();
			paths.push_back( fileInfo.fullPath );
			addFolderItems( paths );
		}
	}
}

ObjectHandle FileSystemAssetBrowserModel::getFolderContents() const
{
	return impl_->folderContents_;
}

ObjectHandle FileSystemAssetBrowserModel::getFolderTreeModel() const
{
	return impl_->folders_.get();
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