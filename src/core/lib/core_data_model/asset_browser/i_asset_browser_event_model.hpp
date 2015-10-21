//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  i_asset_browser_event_model.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef I_ASSET_BROWSER_EVENT_MODEL_H_
#define I_ASSET_BROWSER_EVENT_MODEL_H_

#pragma once
#include "core_reflection/reflected_object.hpp"
#include "core_variant/variant.hpp"

#include <functional>

class IAssetObjectItem;

typedef std::function<void(const Variant&)> VariantCallback;
typedef std::function<void(const bool&)> BoolCallback;
typedef std::function<void(void)> VoidCallback;
typedef std::function<void(const IAssetObjectItem&)> AssetCallback;

class IAssetBrowserEventModel
{
public:
	virtual ~IAssetBrowserEventModel(){}
	virtual void assetSelectionChanged( const Variant& ) {}
	virtual void breadcrumbSelected( const Variant& ) {}
	virtual void contextMenu( const Variant& ) {}
	virtual void filterChanged( const Variant& ) {}
	virtual void folderSelectionChanged( const Variant& ) {}
	virtual void useSelectedAsset( const Variant& ) {}

	virtual void connectAssetSelectionChanged(AssetCallback) {}
	virtual void connectBreadcrumbSelected(VariantCallback) {}
	virtual void connectContextMenu(VariantCallback) {}
	virtual void connectFilterChanged(VariantCallback) {}
	virtual void connectFolderSelectionChanged(VariantCallback) {}
	virtual void connectUseSelectedAsset(AssetCallback) {}

	Variant dummy() const { return Variant(); }
	bool dummyBool() const { return false; }
};

#endif // I_ASSET_BROWSER_EVENT_MODEL_H_