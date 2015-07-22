
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  asset_browser_model.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef ASSET_BROWSER_MODEL_H_
#define ASSET_BROWSER_MODEL_H_

#pragma once

#include "data_model/asset_browser/i_asset_browser_model.hpp"

typedef std::vector<std::string> AssetPaths;

class FileSystemAssetBrowserModel : public IAssetBrowserModel
{
public:
	FileSystemAssetBrowserModel(
		const AssetPaths& assetPaths,
		IFileSystem& fileSystem,
		IDefinitionManager& definitionManager );

	void addAssetPath(const std::string& path);

private:
	
	virtual void initialise(IContextManager& contextManager) override;

	virtual void addListener(IAssetListener* listener) override;

	virtual const AssetPaths& assetPaths() const override;

	virtual void populateFolderContents(const AssetPaths& paths) override;

	virtual ObjectHandle getBreadcrumbs() const override;

	virtual ObjectHandle getFolderContents() const override;

	virtual ObjectHandle getFolderTreeModel() const override;

	virtual bool navigateHistoryForward() const override;

	virtual bool navigateHistoryBackward() const override;

	virtual Variant getFolderTreeItemSelected() const override;

	virtual void setFolderTreeItemSelected(const Variant& selectedItem) override;

	virtual size_t getFolderTreeItemIndex() const override;

	virtual ObjectHandle currentBreadcrumbItemIndex() const override;

	virtual const size_t & getCurrentBreadcrumbItemIndex() const override;

	virtual void setCurrentBreadcrumbItemIndex(const size_t & index) override;

	virtual bool useSelectedAsset() const override;

	virtual const int & currentSelectedAssetIndex() const override;

	virtual void currentSelectedAssetIndex(const int & index) override;

	void addFolderItems(const AssetPaths& paths);

	struct FileSystemAssetBrowserModelImplementation;
	std::unique_ptr<FileSystemAssetBrowserModelImplementation> impl_;
};

#endif // ASSET_BROWSER_MODEL_H_