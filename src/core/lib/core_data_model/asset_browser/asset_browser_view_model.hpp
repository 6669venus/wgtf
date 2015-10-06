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
	AssetBrowserViewModel(
		ObjectHandleT<IAssetBrowserModel> data,
		ObjectHandle contextMenu,
		ObjectHandleT<IAssetBrowserEventModel> events );

	virtual ObjectHandle data() const override;

	virtual ObjectHandle events() const override;

	virtual ObjectHandle contextMenu() const override;

	virtual ObjectHandle getBreadcrumbs() const override;

	virtual size_t getFolderTreeItemIndex() const override;

	virtual ObjectHandle folderSelectionHistoryIndex() const override;

	virtual const size_t & getFolderHistoryIndex() const override;

	virtual void setFolderHistoryIndex( const size_t & index ) override;

	virtual ObjectHandle breadcrumbItemIndexNotifier() const override;

	virtual const size_t & getBreadcrumbItemIndex() const override;

	virtual void setBreadcrumbItemIndex( const size_t & index ) override;

	virtual const int & currentSelectedAssetIndex() const override;

	virtual void currentSelectedAssetIndex( const int & index ) override;

	virtual IAssetObjectModel* getSelectedAssetData() const override;

	virtual ObjectHandle getRecentFileHistory() const override;

	virtual bool refreshData() const override;

	virtual ObjectHandle getFolderSelectionHandler() const override;
	virtual ObjectHandle getFolderContentSelectionHandler() const override;

private:
	void onUseSelectedAsset( const IAssetObjectModel& selectedAsset );
	void updateFolderContentsFilter( const Variant& filter );

	struct AssetBrowserViewModelImplementation;
	std::unique_ptr<AssetBrowserViewModelImplementation> impl_;
};

#endif // ASSET_BROWSER_VIEW_MODEL_H_
