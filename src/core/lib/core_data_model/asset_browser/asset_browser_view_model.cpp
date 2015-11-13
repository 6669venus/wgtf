//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_view_model.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "asset_browser_view_model.hpp"
#include "base_asset_object_item.hpp"
#include "i_asset_browser_model.hpp"
#include "i_asset_browser_event_model.hpp"
#include "i_asset_browser_context_menu_model.hpp"
#include "asset_browser_breadcrumbs_model.hpp"

#include "core_data_model/variant_list.hpp"
#include "core_data_model/value_change_notifier.hpp"
#include "core_data_model/i_tree_model.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/selection_handler.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_serialization/interfaces/i_file_system.hpp"

#include <sstream>
#include <stdio.h>

static const size_t NO_SELECTION = SIZE_MAX;

struct AssetBrowserViewModel::AssetBrowserViewModelImplementation
{
	AssetBrowserViewModelImplementation(
		IDefinitionManager& definitionManager,
		ObjectHandleT<IAssetBrowserModel> data,
		ObjectHandleT<IAssetBrowserContextMenuModel> contextMenu,
		ObjectHandleT<IAssetBrowserEventModel> events)
		: definitionManager_( definitionManager )
		, currentSelectedAssetIndex_( -1 )
		, currentFolderHistoryIndex_( NO_SELECTION )
		, selectedTreeItem_(nullptr)
		, ignoreFolderHistory_( false )
		, contextMenu_( std::move(contextMenu) )
		, data_( std::move(data) )
		, events_( std::move(events) )
		, breadcrumbsModel_( nullptr )
	{
		folderSelectionHandler_.onPostSelectionChanged().add< AssetBrowserViewModel::AssetBrowserViewModelImplementation,
		&AssetBrowserViewModel::AssetBrowserViewModelImplementation::onPostFolderDataChanged >( this );
		folderContentSelectionHandler_.onPostSelectionChanged().add< AssetBrowserViewModel::AssetBrowserViewModelImplementation,
			&AssetBrowserViewModel::AssetBrowserViewModelImplementation::onPostFolderContentDataChanged >( this );

		breadcrumbsModel_ = std::unique_ptr< AssetBrowserBreadcrumbsModel >( new AssetBrowserBreadcrumbsModel( definitionManager ) );
	}

	~AssetBrowserViewModelImplementation()
	{
		folderSelectionHandler_.onPostSelectionChanged().remove< AssetBrowserViewModel::AssetBrowserViewModelImplementation,
			&AssetBrowserViewModel::AssetBrowserViewModelImplementation::onPostFolderDataChanged >( this );
		folderContentSelectionHandler_.onPostSelectionChanged().remove< AssetBrowserViewModel::AssetBrowserViewModelImplementation,
			&AssetBrowserViewModel::AssetBrowserViewModelImplementation::onPostFolderContentDataChanged >( this );
	}

	/// Rebuild the breadcrumb from fullpath
	void rebuildBreadcrumb( const char* value )
	{
		breadcrumbsModel_->clear();
		breadcrumbsModel_->setPath( value );
		
		std::string	tmpPath = value;
		std::string::size_type firstIndex = 0;

		std::string originalRootPath = "";
		std::string rootPath = "";
		auto assetPaths = data_->assetPaths();
		for (auto& path : assetPaths)
		{
			// Find the appropriate root to use for this asset's path
			if (strstr( value, rootPath.c_str() ) != nullptr)
			{
				originalRootPath = path;
				rootPath = path;
				break;
			}
		}

		// Convert the root path to use the alt directory seperator to make this compatible with non-Windows systems.
		std::replace( rootPath.begin(), rootPath.end(), FilePath::kAltDirectorySeparator,
			FilePath::kDirectorySeparator );

		// Workaround of the file system returning the root path without kAltDirectorySeparator
		std::string::size_type directorySeperatorIndex = tmpPath.find( FilePath::kDirectorySeparator );
		if (std::string::npos == directorySeperatorIndex)
		{
			// Replace the directory separator, '/', with the alt directory separator, '\\'
			std::replace( rootPath.begin(), rootPath.end(), FilePath::kDirectorySeparator,
				FilePath::kAltDirectorySeparator );
		}

		// Find and remove the root path and normalize the directory seperator to use the accepted tokenizer format.
		// This is done, because some root paths may have extended pathing such as "../../res/game/", and they must be
		// treated as a single string for breadcrumbs and cannot be tokenized along with the rest of the path.
		firstIndex = tmpPath.find( rootPath.c_str() );
		if (std::string::npos == firstIndex)
		{
			// Malformed path in the FileInfo!
			return;
		}

		// Fetch the temporary path using the rootPath
		tmpPath.erase( firstIndex, rootPath.length() );
		std::replace( tmpPath.begin(), tmpPath.end(), FilePath::kDirectorySeparator, FilePath::kAltDirectorySeparator );
		
		// Add the root breadcrumb before tokenizing the rest.
		IAssetObjectItem* breadcrumbRootItem = data_->getAssetAtPath( originalRootPath.c_str() );
		breadcrumbsModel_->add( breadcrumbRootItem );
		
		// Keep track of the working path as we iterate over the tokens so that breadcrumbs can be built
		// intelligently off their IAssetObjectItems
		std::stringstream workingPath;
		workingPath << rootPath;

		// Tokenize the remaining portion of the path and create presentable breadcrumb strings that
		// will correspond to navigation history
		std::istringstream stream( tmpPath );
		std::string token;
		while (std::getline( stream, token, (char)FilePath::kAltDirectorySeparator ))
		{
			if (token.length() > 0)
			{
				if (breadcrumbsModel_->size() > 1)
				{
					workingPath << FilePath::kAltDirectorySeparator;
				}
				workingPath << token;
							
				IAssetObjectItem* tokenItem = data_->getAssetAtPath( workingPath.str().c_str() );
				breadcrumbsModel_->add( tokenItem );
			}
		}
	}

	void generateBreadcrumbs( const IItem* selectedItem )
	{
		auto folders = data_->getFolderTreeModel();
		if (selectedItem && folders)
		{
			// Rebuild the breadcrumb each time to support the breadcrumb click navigation
			auto variant = selectedItem->getData( 0, IndexPathRole::roleId_ );
			if (variant.canCast< std::string >())
			{
				rebuildBreadcrumb( variant.cast< std::string >().c_str() );
			}

			// Reset the flag
			ignoreFolderHistory_ = false;
		}
	}

	void onPostFolderDataChanged( const ISelectionHandler* sender,
		const ISelectionHandler::PostSelectionChangedArgs& args )
	{
		std::vector< IItem* > items = folderSelectionHandler_.getSelectedItems();
		if (items.empty())
		{
			return;
		}

		assert( items.size() == 1);

		selectedTreeItem_ = items[0];
		data_.get()->populateFolderContents( selectedTreeItem_ );

		this->generateBreadcrumbs( selectedTreeItem_ );
	}

	void onPostFolderContentDataChanged( const ISelectionHandler* sender,
		const ISelectionHandler::PostSelectionChangedArgs& args )
	{
		std::vector< int > indices = folderSelectionHandler_.getSelectedRows();
		if (indices.empty())
		{
			return;
		}

		assert( indices.size() == 1);

		currentSelectedAssetIndex_ = indices[0];
	}

	IDefinitionManager& definitionManager_;
	int					currentSelectedAssetIndex_;
	size_t				currentFolderHistoryIndex_;
	IItem*				selectedTreeItem_;
	bool				ignoreFolderHistory_;

	ObjectHandleT<IAssetBrowserContextMenuModel>	contextMenu_;
	ObjectHandleT<IAssetBrowserModel>				data_;
	ObjectHandleT<IAssetBrowserEventModel>			events_;
	std::unique_ptr<AssetBrowserBreadcrumbsModel>	breadcrumbsModel_;

	SelectionHandler folderSelectionHandler_;
	SelectionHandler folderContentSelectionHandler_;
};

AssetBrowserViewModel::AssetBrowserViewModel(
	IDefinitionManager& definitionManager,
	ObjectHandleT<IAssetBrowserModel> data,
	ObjectHandleT<IAssetBrowserContextMenuModel> contextMenu,
	ObjectHandleT<IAssetBrowserEventModel> events ) :
	impl_( new AssetBrowserViewModelImplementation( definitionManager, std::move(data), 
			std::move(contextMenu), std::move(events) ) )
{
	if(impl_->events_.get())
	{
		impl_->events_->connectFilterChanged( [&]( const Variant& filter ) { updateFolderContentsFilter( filter ); } );
	}
}

ObjectHandle AssetBrowserViewModel::data() const
{
	return impl_->data_;
}

ObjectHandle AssetBrowserViewModel::events() const
{
	return impl_->events_;
}

ObjectHandle AssetBrowserViewModel::contextMenu() const
{
	return impl_->contextMenu_;
}

IBreadcrumbsModel * AssetBrowserViewModel::getBreadcrumbsModel() const
{
	return impl_->breadcrumbsModel_.get();
}

const int & AssetBrowserViewModel::currentSelectedAssetIndex() const
{
	return impl_->currentSelectedAssetIndex_;
}

void AssetBrowserViewModel::currentSelectedAssetIndex( const int & index )
{
	impl_->currentSelectedAssetIndex_ = index;
}

IAssetObjectItem* AssetBrowserViewModel::getSelectedAssetData() const
{
	//TODO: This will need to support multi-selection. Right now it is a single item
	// selection, but the QML is the same way.
	auto dataModel = impl_->data_.get();
	if (dataModel != nullptr)
	{
		return dataModel->getFolderContentsAtIndex( impl_->currentSelectedAssetIndex_ );
	}

	return nullptr;
}

bool AssetBrowserViewModel::refreshData() const
{
	if (impl_->selectedTreeItem_ != nullptr)
	{
		impl_->data_->populateFolderContents( impl_->selectedTreeItem_ );
	}

	return true;
}

void AssetBrowserViewModel::updateFolderContentsFilter( const Variant& filter )
{
	std::string newFilter = "";
	if (filter.typeIs<std::string>())
	{
		bool isOk = filter.tryCast( newFilter );
		if (isOk)
		{
			// Set the new filter for folder contents
			impl_->data_->setFolderContentsFilter( newFilter );

			// Refresh is required to apply the new filter
			refreshData();
		}
	}
}

ISelectionHandler * AssetBrowserViewModel::getFolderSelectionHandler() const
{
	return &impl_->folderSelectionHandler_;
}

ISelectionHandler * AssetBrowserViewModel::getFolderContentSelectionHandler() const
{
	return &impl_->folderContentSelectionHandler_;
}
