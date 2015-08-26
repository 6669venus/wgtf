
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_model.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "file_system_asset_browser_model.hpp"

#include "core_data_model/asset_browser/file_object_model.hpp"
#include "core_data_model/asset_browser/folder_tree_item.hpp"
#include "core_data_model/asset_browser/folder_tree_model.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/i_tree_model.hpp"
#include "core_data_model/value_change_notifier.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_logging/logging.hpp"
#include "core_serialization/interfaces/i_file_system.hpp"
#include "core_reflection/type_class_definition.hpp"

#include <list>

static const int NO_SELECTION = -1;

struct FileSystemAssetBrowserModel::FileSystemAssetBrowserModelImplementation
{
	FileSystemAssetBrowserModelImplementation(
		FileSystemAssetBrowserModel& self,
		IFileSystem& fileSystem,
		IDefinitionManager& definitionManager )
		: self_( self )
		, folders_( nullptr )
		, folderContentsFilter_( "" )
		, contentFilterIndexNotifier_( NO_SELECTION )
		, currentCustomFilterIndex_( -1 )
		, definitionManager_( definitionManager )
		, fileSystem_( fileSystem )
	{
	}

	void addFolderItem(const FileInfo& fileInfo)
	{
		if (self_.fileHasFilteredExtension(fileInfo))
		{
			auto assetObjectDef = definitionManager_.getDefinition<IAssetObjectModel>();
			if(assetObjectDef)
			{
				auto object = TypeClassDefinition<FileObjectModel>::create(*assetObjectDef, fileInfo);
				folderContents_.push_back( object );
			}
		}
	}

	IAssetObjectModel* getFolderContentsAtIndex( const int & index )
	{
		if (index < 0 || index >= (int)folderContents_.size())
		{
			return nullptr;
		}

		auto genericItem = static_cast< VariantListItem* >( folderContents_.item( index ) );
		if (genericItem != nullptr)
		{
			Variant variant = genericItem->value< ObjectHandle >();			
			if (variant.typeIs< ObjectHandle >())
			{
				ObjectHandle object;
				if (variant.tryCast( object ))
				{
					return object.getBase< IAssetObjectModel >();
				}
			}
		}

		return nullptr;
	}

	FileSystemAssetBrowserModel& self_;
	VariantList	folderContents_;
	VariantList customContentFilters_;
	std::shared_ptr<ITreeModel>	folders_;

	IDefinitionManager&	definitionManager_;
	IFileSystem&		fileSystem_;
	AssetPaths			assetPaths_;
	std::string			folderContentsFilter_;

	ValueChangeNotifier< int >	contentFilterIndexNotifier_;
	int							currentCustomFilterIndex_;
};

FileSystemAssetBrowserModel::FileSystemAssetBrowserModel(
	const AssetPaths& assetPaths, const CustomContentFilters& customContentFilters, 
	IFileSystem& fileSystem, IDefinitionManager& definitionManager )
	: impl_(new FileSystemAssetBrowserModelImplementation( *this, fileSystem, definitionManager ) )
{
	for (auto& path : assetPaths)
	{
		addAssetPath( path );
	}

	for (auto& filter : customContentFilters)
	{
		addCustomContentFilter( filter );
	}

	// Create the FolderTreeModel now that we've added our asset paths
	impl_->folders_.reset( new FolderTreeModel( *this, impl_->fileSystem_ ) );
}

void FileSystemAssetBrowserModel::addAssetPath(const std::string& path)
{
	if (std::find(impl_->assetPaths_.begin(), impl_->assetPaths_.end(), path)
		== impl_->assetPaths_.end())
	{
		if (!impl_->fileSystem_.exists(path.c_str()))
		{
			NGT_ERROR_MSG( "TestAssetBrowserModel::addAssetPath: "
				"asset folder path does not exist: %s\n", path.c_str() );
			return;
		}

		impl_->assetPaths_.push_back( path );
	}
}

void FileSystemAssetBrowserModel::addCustomContentFilter( const std::string& filter )
{
	impl_->customContentFilters_.push_back( filter.c_str() );
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


bool FileSystemAssetBrowserModel::fileHasFilteredExtension( const FileInfo& fileInfo )
{
	std::string fileExtensionFilter;
	getSelectedCustomFilterText( fileExtensionFilter );
	
	if (fileExtensionFilter.length() < 1 || fileExtensionFilter.compare( "*.*" ) == 0)
	{
		// No filter being applied.
		// Note: Qt ComboBox does not support selecting an empty string value. *.* is hardcoded until a better
		//       solution is made available.
		return true;
	}

	return ( std::strcmp( fileInfo.extension(), fileExtensionFilter.c_str() ) == 0 );
}

IAssetObjectModel* FileSystemAssetBrowserModel::getFolderContentsAtIndex( const int & index ) const
{
	return impl_->getFolderContentsAtIndex( index );
}

void FileSystemAssetBrowserModel::getSelectedCustomFilterText( std::string & value ) const
{
	// Note: Since it is likely this particular feature will be rolled into a more robust filtering
	//       system later, it will not be plopped into the impl_ to make it easier to remove later.
	int index = impl_->currentCustomFilterIndex_;
	if (index < 0 || index >= ( int )impl_->customContentFilters_.size())
	{
		return;
	}

	auto genericItem = static_cast< VariantListItem* >( impl_->customContentFilters_.item( index ) );
	if (genericItem != nullptr)
	{
		Variant variant = genericItem->value< std::string >();	
		if (variant.typeIs< const char * >() ||
			variant.typeIs< std::string >())
		{
			variant.tryCast( value );
		}
	}
}

ObjectHandle FileSystemAssetBrowserModel::getFolderContents() const
{
	return static_cast< IListModel & >( impl_->folderContents_ );
}

ObjectHandle FileSystemAssetBrowserModel::getFolderTreeModel() const
{
	return impl_->folders_.get();
}

ObjectHandle FileSystemAssetBrowserModel::getCustomContentFilters() const
{
	return impl_->customContentFilters_;
}

const int & FileSystemAssetBrowserModel::currentCustomContentFilter() const
{
	return impl_->currentCustomFilterIndex_;
}

void FileSystemAssetBrowserModel::currentCustomContentFilter( const int & index )
{
	impl_->currentCustomFilterIndex_ = index;
	impl_->contentFilterIndexNotifier_.value( index );
}

void FileSystemAssetBrowserModel::setFolderContentsFilter( const std::string filter )
{
	impl_->folderContentsFilter_ = filter;
}

ObjectHandle FileSystemAssetBrowserModel::customContentFilterIndexNotifier() const
{
	return ObjectHandle( &impl_->contentFilterIndexNotifier_ );
}

void FileSystemAssetBrowserModel::addFolderItems( const AssetPaths& paths )
{
	IFileSystem& fs = impl_->fileSystem_;

	std::list<std::string> directories;

	for (auto& path : paths)
	{
		if (!fs.exists(path.c_str()))
		{
			NGT_WARNING_MSG("FileSystemAssetBrowserModel::addFolderItems: "
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
