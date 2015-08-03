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

#include "reflection/reflected_object.hpp"

//------------------------------------------------------------------------------
// IAssetBrowserViewModel
//
// Represents the view model for the WGAssetBrowser control. Provides the
// data model used for the tree and list views as well as an event model.
// Provides properties used by the view based on the provided data model.
//------------------------------------------------------------------------------

class IAssetBrowserViewModel
{
	DECLARE_REFLECTED

public:
	IAssetBrowserViewModel() : tempSizeT_( 0 ), tempInt_( 0 )
	{
	}

	virtual ~IAssetBrowserViewModel(){}


	// Retrieve the data model
	// Expected: IAssetBrowserViewModel
	virtual ObjectHandle view() const { return this; }

	// Retrieve the data model
	// Expected: IAssetBrowserModel
	virtual ObjectHandle data() const { return ObjectHandle(); }

	// Retrieve the event model
	// Expected: IAssetBrowserEventModel
	virtual ObjectHandle events() const { return ObjectHandle(); }

	// Retrieve the breadcrumbs
	// Expected: IListModel
	virtual ObjectHandle getBreadcrumbs() const { return ObjectHandle(); }

	// Folder tree view selection handlers
	virtual Variant getFolderTreeItemSelected() const { return Variant(); }
	virtual void setFolderTreeItemSelected( const Variant& selectedItem ) {};
	virtual size_t getFolderTreeItemIndex() const { return tempSizeT_; }

	// Breadcrumb selection index accessor/mutator
	virtual ObjectHandle currentBreadcrumbItemIndex() const { return ObjectHandle(); }
	virtual const size_t & getCurrentBreadcrumbItemIndex() const { return tempSizeT_; }
	virtual void setCurrentBreadcrumbItemIndex( const size_t & index ) {};

	// Asset usage handlers (note: pattern likely to change in future iterations)
	virtual bool useSelectedAsset() const { return true; }
	virtual const int & currentSelectedAssetIndex() const { return tempInt_; }
	virtual void currentSelectedAssetIndex( const int & index ) {};

private:
	size_t tempSizeT_;
	int tempInt_;
};

#endif // I_ASSET_BROWSER_VIEW_MODEL_H_