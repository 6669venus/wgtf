//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_event_model.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "asset_browser_event_model.hpp"
#include "core_variant/variant.hpp"

void AssetBrowserEventModel::assetSelectionChanged(const Variant& selection)
{
	onAssetSelectionChanged(selection);
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

void AssetBrowserEventModel::navigateHistoryBackward(const bool& val)
{
	onNavigateHistoryBackward(val);
}

void AssetBrowserEventModel::useSelectedAsset(const Variant& asset)
{
	onUseSelectedAsset(asset);
}

void AssetBrowserEventModel::connectAssetSelectionChanged(VariantCallback callback)
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

void AssetBrowserEventModel::connectNavigateHistoryBackward(BoolCallback callback)
{
	onNavigateHistoryBackward.connect(callback);
}

void AssetBrowserEventModel::connectUseSelectedAsset(VariantCallback callback)
{
	onUseSelectedAsset.connect(callback);
}
