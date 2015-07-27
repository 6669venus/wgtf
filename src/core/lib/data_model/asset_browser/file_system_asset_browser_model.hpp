
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

class IFileSystem;
class IDefinitionManager;

class FileSystemAssetBrowserModel : public IAssetBrowserModel
{
public:
	FileSystemAssetBrowserModel(
		const AssetPaths& assetPaths,
		IFileSystem& fileSystem,
		IDefinitionManager& definitionManager );

	void addAssetPath(const std::string& path);

	virtual const AssetPaths& assetPaths() const;

private:
	
	virtual void initialise(IContextManager& contextManager) override;
	
	virtual void populateFolderContents(const IItem* item) override;

	virtual ObjectHandle getFolderContents() const override;

	virtual ObjectHandle getFolderTreeModel() const override;

	void addFolderItems(const AssetPaths& paths);

	struct FileSystemAssetBrowserModelImplementation;
	std::unique_ptr<FileSystemAssetBrowserModelImplementation> impl_;
};

#endif // ASSET_BROWSER_MODEL_H_