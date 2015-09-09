//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  i_asset_browser_view_model.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef I_ASSET_BROWSER_VIEW_MODEL_H_
#define I_ASSET_BROWSER_VIEW_MODEL_H_

#pragma once

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_variant/variant.hpp"

class IAssetObjectModel;

//------------------------------------------------------------------------------
// IAssetBrowserViewModel
//
// Represents the view model for the WGAssetBrowser control. Provides the
// data model used for the tree and list views as well as an event model.
// Provides properties used by the view based on the provided data model.
//------------------------------------------------------------------------------

class IAssetBrowserViewModel
{
public:
	IAssetBrowserViewModel() : tempSizeT_( 0 ), tempInt_( 0 )
	{
	}

	virtual ~IAssetBrowserViewModel(){}


	// Retrieve the view model
	// Expected: IAssetBrowserViewModel
	virtual ObjectHandle view() const { return this; }

	// Retrieve the data model
	// Expected: IAssetBrowserModel
	virtual ObjectHandle data() const { return ObjectHandle(); }

	// Retrieve the event model
	// Expected: IAssetBrowserEventModel
	virtual ObjectHandle events() const { return ObjectHandle(); }

	// Retrieve the context menu model
	// Expected: Backing view model for custom context menu
	virtual ObjectHandle contextMenu() const { return ObjectHandle(); }

	// Retrieve the breadcrumbs
	// Expected: IListModel
	virtual ObjectHandle getBreadcrumbs() const { return ObjectHandle(); }

	// Folder tree view selection handlers
	virtual ObjectHandle getFolderSelectionHandler() const { return ObjectHandle(); }
	virtual ObjectHandle getFolderContentSelectionHandler() const { return ObjectHandle(); }
	virtual size_t getFolderTreeItemIndex() const { return tempSizeT_; }

	// Breadcrumb selection index accessor/mutator
	virtual ObjectHandle folderSelectionHistoryIndex() const { return ObjectHandle(); }
	virtual const size_t & getFolderHistoryIndex() const { return tempSizeT_; }
	virtual void setFolderHistoryIndex( const size_t & index ) {};
	virtual ObjectHandle breadcrumbItemIndexNotifier() const { return ObjectHandle(); }
	virtual const size_t & getBreadcrumbItemIndex() const { return tempSizeT_; };
	virtual void setBreadcrumbItemIndex( const size_t & index ) {};

	// Asset usage handlers (note: pattern likely to change in future iterations)
	virtual bool useSelectedAsset() const { return true; }
	virtual const int & currentSelectedAssetIndex() const { return tempInt_; }
	virtual void currentSelectedAssetIndex( const int & index ) {}

	// Retrieve the selected asset data - not exposed to QML. For native-use only.
	virtual IAssetObjectModel*  getSelectedAssetData() const { return nullptr; }

	// Retrieve the recently used file history
	// Expected: IListModel
	virtual ObjectHandle getRecentFileHistory() const { return ObjectHandle(); }

	// Invokes a refresh of the data models based on plugin states. How the refresh is handled is
	// entirely up to the developer.
	// Expected: Boolean
	virtual bool refreshData() const { return true; }


private:
	size_t tempSizeT_;
	int tempInt_;
};

#endif // I_ASSET_BROWSER_VIEW_MODEL_H_