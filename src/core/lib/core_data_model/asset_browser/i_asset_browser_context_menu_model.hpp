//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  i_context_menu_model.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef I_ASSET_BROWSER_CONTEXT_MENU_MODEL_H_
#define I_ASSET_BROWSER_CONTEXT_MENU_MODEL_H_

#pragma once

#include "core_reflection/reflected_object.hpp"

class IAssetBrowserViewModel;

//------------------------------------------------------------------------------
// IAssetBrowserContextMenuModel
//
// Represents a context menu data, methods, and properties. 
//
// NOTE: It is far too specific to the asset browser and will require 
// additional work to make generic for other plugins. Ideally this will 
// inherit from another data model, with the generic information and the
// asset browser users will implement on top of this model with their 
// specific functionality and building dynamic menus.
//
// JIRA: http://jira.bigworldtech.com/browse/NGT-905
//------------------------------------------------------------------------------

class IAssetBrowserContextMenuModel
{
	DECLARE_REFLECTED

public:
	IAssetBrowserContextMenuModel(){}
	virtual ~IAssetBrowserContextMenuModel(){}

	virtual void setViewModel( IAssetBrowserViewModel * viewModel ) = 0;
};

#endif // I_ASSET_BROWSER_CONTEXT_MENU_MODEL_H_