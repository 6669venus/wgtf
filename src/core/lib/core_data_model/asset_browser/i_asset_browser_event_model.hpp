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
#include "reflection/reflected_object.hpp"
#include "variant/variant.hpp"

#include <functional>

typedef std::function<void(const Variant&)> VariantCallback;
typedef std::function<void(const bool&)> BoolCallback;
typedef std::function<void(void)> VoidCallback;

class IAssetBrowserEventModel
{
	DECLARE_REFLECTED

public:
	virtual ~IAssetBrowserEventModel(){}
	virtual void assetSelectionChanged( const Variant& ) {}
	virtual void breadcrumbSelected( const Variant& ) {}
	virtual void contextMenu( const Variant& ) {}
	virtual void filterChanged( const Variant& ) {}
	virtual void folderSelectionChanged( const Variant& ) {}
	virtual void navigateHistoryForward( const bool& ) { }
	virtual void navigateHistoryBackward( const bool& ) { }
	virtual void useSelectedAsset( const Variant& ) {}

	virtual void connectAssetSelectionChanged(VariantCallback) {}
	virtual void connectBreadcrumbSelected(VariantCallback) {}
	virtual void connectContextMenu(VariantCallback) {}
	virtual void connectFilterChanged(VariantCallback) {}
	virtual void connectFolderSelectionChanged(VariantCallback) {}
	virtual void connectNavigateHistoryForward(VoidCallback) { }
	virtual void connectNavigateHistoryBackward(BoolCallback) { }
	virtual void connectUseSelectedAsset(VariantCallback) {}

	Variant unused;
	const Variant& dummy() const { return unused; }
	bool dummyBool() const { return false; }
};

#endif // I_ASSET_BROWSER_EVENT_MODEL_H_