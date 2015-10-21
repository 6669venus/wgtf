//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_event_model.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef ASSET_BROWSER_EVENT_MODEL_H_
#define ASSET_BROWSER_EVENT_MODEL_H_

#pragma once

#include "i_asset_browser_event_model.hpp"
#include "core_common/signal.hpp"

#include <vector>

typedef Signal<void(const Variant&)> SignalVariant;
typedef Signal<void(bool)> SignalBool;
typedef Signal<void(void)> SignalVoid;
typedef Signal<void(const IAssetObjectItem&)> SignalAsset;

class AssetBrowserEventModel : public Implements<IAssetBrowserEventModel>
{
public:
	virtual void assetSelectionChanged(const Variant& selection) override;
	virtual void breadcrumbSelected(const Variant& breadcrumb) override;
	virtual void contextMenu(const Variant& menu) override;
	virtual void folderSelectionChanged(const Variant& folderSelection) override;
	virtual void useSelectedAsset(const Variant& asset) override;

	virtual void connectAssetSelectionChanged(AssetCallback) override;
	virtual void connectBreadcrumbSelected(VariantCallback) override;
	virtual void connectContextMenu(VariantCallback) override;
	virtual void connectFolderSelectionChanged(VariantCallback) override;
	virtual void connectUseSelectedAsset(AssetCallback) override;

private:
	SignalAsset onAssetSelectionChanged;
	SignalVariant onBreadcrumbSelected;
	SignalVariant onContextMenu;
	SignalVariant onFolderSelectionChanged;
	SignalAsset onUseSelectedAsset;
};
#endif // ASSET_BROWSER_EVENT_MODEL_H_