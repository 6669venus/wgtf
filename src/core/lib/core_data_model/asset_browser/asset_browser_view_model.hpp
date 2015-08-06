//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_view_model.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef ASSET_BROWSER_VIEW_MODEL_H_
#define ASSET_BROWSER_VIEW_MODEL_H_

#pragma once

#include "i_asset_browser_view_model.hpp"

class IAssetBrowserModel;
class IAssetBrowserEventModel;

class AssetBrowserViewModel : public IAssetBrowserViewModel
{
public:
	AssetBrowserViewModel( ObjectHandle data, ObjectHandle events );

	virtual ObjectHandle data() const override;

	virtual ObjectHandle events() const override;

	virtual ObjectHandle getBreadcrumbs() const override;

	virtual Variant getFolderTreeItemSelected() const override;

	virtual void setFolderTreeItemSelected( const Variant& selectedItem ) override;

	virtual size_t getFolderTreeItemIndex() const override;

	virtual ObjectHandle currentBreadcrumbItemIndex() const override;

	virtual const size_t & getCurrentBreadcrumbItemIndex() const override;

	virtual void setCurrentBreadcrumbItemIndex( const size_t & index ) override;

	virtual const int & currentSelectedAssetIndex() const override;

	virtual void currentSelectedAssetIndex( const int & index ) override;

private:
	void onNavigateHistoryForward();
	void onNavigateHistoryBackward();

	struct AssetBrowserViewModelImplementation;
	std::unique_ptr<AssetBrowserViewModelImplementation> impl_;
};

#endif // ASSET_BROWSER_VIEW_MODEL_H_