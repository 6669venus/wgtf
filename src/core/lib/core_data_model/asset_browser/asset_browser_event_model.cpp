//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_event_model.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "asset_browser_event_model.hpp"
#include "core_variant/variant.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_reflection/object_handle.hpp"
#include "i_asset_object_model.hpp"

template<class Type>
Type* get(const Variant &selectedAsset)
{
	auto genericListItem = reinterpret_cast<IItem*>( selectedAsset.value<intptr_t>() );

	if ( genericListItem == nullptr )
		return nullptr;

	auto selectedElement = genericListItem->getData(0, ValueRole::roleId_).value<ObjectHandleT<Type>>();
	return selectedElement.get();
}

void AssetBrowserEventModel::assetSelectionChanged(const Variant& selectedAsset)
{
	auto asset = get<IAssetObjectModel>(selectedAsset);
	if ( asset )
		onAssetSelectionChanged(*asset);
}

void AssetBrowserEventModel::breadcrumbSelected(const Variant& breadcrumb)
{
	onBreadcrumbSelected(breadcrumb);
}

void AssetBrowserEventModel::contextMenu(const Variant& menu)
{
	onContextMenu(menu);
}

void AssetBrowserEventModel::filterChanged(const Variant& filter)
{
	onFilterChanged(filter);
}

void AssetBrowserEventModel::folderSelectionChanged(const Variant& folderSelection)
{
	onFolderSelectionChanged(folderSelection);
}

void AssetBrowserEventModel::navigateHistoryForward(const bool&)
{
	onNavigateHistoryForward();
}

void AssetBrowserEventModel::navigateHistoryBackward(const bool&)
{
	onNavigateHistoryBackward();
}

void AssetBrowserEventModel::useSelectedAsset(const Variant& selectedAsset)
{
	auto asset = get<IAssetObjectModel>(selectedAsset);
	if(asset)
		onUseSelectedAsset(*asset);
}


void AssetBrowserEventModel::connectAssetSelectionChanged(AssetCallback callback)
{
	onAssetSelectionChanged.connect(callback);
}

void AssetBrowserEventModel::connectBreadcrumbSelected(VariantCallback callback)
{
	onBreadcrumbSelected.connect(callback);
}

void AssetBrowserEventModel::connectContextMenu(VariantCallback callback)
{
	onContextMenu.connect(callback);
}

void AssetBrowserEventModel::connectFilterChanged(VariantCallback callback)
{
	onFilterChanged.connect(callback);
}

void AssetBrowserEventModel::connectFolderSelectionChanged(VariantCallback callback)
{
	onFolderSelectionChanged.connect(callback);
}

void AssetBrowserEventModel::connectNavigateHistoryForward(VoidCallback callback) 
{
	onNavigateHistoryForward.connect(callback);
}

void AssetBrowserEventModel::connectNavigateHistoryBackward(VoidCallback callback)
{
	onNavigateHistoryBackward.connect(callback);
}

void AssetBrowserEventModel::connectUseSelectedAsset(AssetCallback callback)
{
	onUseSelectedAsset.connect(callback);
}

