
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

#include "core_data_model/asset_browser/i_asset_browser_model.hpp"

typedef std::vector<std::string> AssetPaths;
typedef std::vector<std::string> CustomContentFilters;

class IFileSystem;
struct FileInfo;
class IDefinitionManager;

class FileSystemAssetBrowserModel : public IAssetBrowserModel
{
public:
	FileSystemAssetBrowserModel(
		const AssetPaths& assetPaths,
		const CustomContentFilters& customContentFilters,
		IFileSystem& fileSystem,
		IDefinitionManager& definitionManager );

	void addAssetPath( const std::string& path );
	
	void addCustomContentFilter( const std::string& filter );

	virtual const AssetPaths& assetPaths() const override;

	virtual const int & currentCustomContentFilter() const override;
	virtual void currentCustomContentFilter( const int & index ) override;

	virtual void setFolderContentsFilter( const std::string filter ) override;

private:

	virtual bool fileHasFilteredExtension( const FileInfo& fileInfo );

	virtual void initialise( IComponentContext& contextManager, IDefinitionManager& definitionManager ) override;
	
	virtual void populateFolderContents( const IItem* item ) override;

	virtual IAssetObjectModel* getFolderContentsAtIndex( const int & index ) const override;

	virtual void getSelectedCustomFilterText( std::string & value ) const;

	virtual ObjectHandle getFolderContents() const override;

	virtual ObjectHandle getFolderTreeModel() const override;

	virtual ObjectHandle getCustomContentFilters() const override;

	virtual ObjectHandle customContentFilterIndexNotifier() const override;

	virtual ObjectHandle getActiveFiltersModel() const override;

	void addFolderItems( const AssetPaths& paths );

	struct FileSystemAssetBrowserModelImplementation;
	std::unique_ptr<FileSystemAssetBrowserModelImplementation> impl_;
};

#endif // ASSET_BROWSER_MODEL_H_