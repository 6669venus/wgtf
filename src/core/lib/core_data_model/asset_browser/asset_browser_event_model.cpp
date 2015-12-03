//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_event_model.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "asset_browser_event_model.hpp"
#include "base_asset_object_item.hpp"
#include "core_variant/variant.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_reflection/object_handle.hpp"

template<class Type>
Type* get( const Variant & selectedAsset )
{
	auto listItem = reinterpret_cast< IItem* >( selectedAsset.value<intptr_t>() );
	return dynamic_cast< Type* >( listItem );
}

void AssetBrowserEventModel::assetSelectionChanged( const Variant & selectedAsset )
{
	auto asset = get< IAssetObjectItem >( selectedAsset );
	if (asset)
		onAssetSelectionChanged( *asset );
}

void AssetBrowserEventModel::breadcrumbSelected( const Variant & breadcrumb )
{
	onBreadcrumbSelected( breadcrumb );
}

void AssetBrowserEventModel::folderSelectionChanged( const Variant & folderSelection )
{
	onFolderSelectionChanged(folderSelection);
}

void AssetBrowserEventModel::useSelectedAsset( const Variant & selectedAsset )
{	
	auto asset = get< IAssetObjectItem >( selectedAsset );
	if (asset)
		onUseSelectedAsset( *asset );
}

void AssetBrowserEventModel::connectAssetSelectionChanged( AssetCallback callback )
{
	onAssetSelectionChanged.connect( callback );
}

void AssetBrowserEventModel::connectBreadcrumbSelected( VariantCallback callback )
{
	onBreadcrumbSelected.connect( callback );
}

void AssetBrowserEventModel::connectFolderSelectionChanged( VariantCallback callback )
{
	onFolderSelectionChanged.connect( callback );
}

void AssetBrowserEventModel::connectUseSelectedAsset( AssetCallback callback )
{
	onUseSelectedAsset.connect( callback );
}

