
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
class IAssetPresentationProvider;

class FileSystemAssetBrowserModel : public IAssetBrowserModel
{
public:
	FileSystemAssetBrowserModel(
		const AssetPaths& assetPaths,
		const CustomContentFilters& customContentFilters,
		IFileSystem& fileSystem,
		IDefinitionManager& definitionManager,
		IAssetPresentationProvider& presentationProvider );
	~FileSystemAssetBrowserModel();

	void addAssetPath( const std::string& path );
	
	void addCustomContentFilter( const std::string& filter );

	virtual const AssetPaths& assetPaths() const override;

	virtual const int & currentCustomContentFilter() const override;
	virtual void currentCustomContentFilter( const int & index ) override;

	virtual void setFolderContentsFilter( const std::string filter ) override;

	virtual Variant findAssetWithPath( std::string path ) override;

	virtual IAssetObjectItem* getAssetAtPath( const char * path, IAssetObjectItem * parent = nullptr ) const override;
	virtual const int& getIconSize() const override;
	virtual void setIconSize(const int&) override;

private:

	virtual bool fileHasFilteredExtension( const FileInfo& fileInfo );

	virtual void initialise( IComponentContext& contextManager, IDefinitionManager& definitionManager ) override;

	virtual void finalise() override;

	virtual void populateFolderContents( const IItem* item ) override;

	virtual IAssetObjectItem* getFolderContentsAtIndex( const int & index ) const override;

	virtual void getSelectedCustomFilterText( std::string & value ) const;

	virtual IListModel * getFolderContents() const override;

	virtual ITreeModel * getFolderTreeModel() const override;

	virtual IListModel * getCustomContentFilters() const override;

	virtual IValueChangeNotifier * customContentFilterIndexNotifier() const override;

	virtual IActiveFiltersModel * getActiveFiltersModel() const override;

	void addFolderItems( const AssetPaths& paths );

	struct FileSystemAssetBrowserModelImplementation;
	std::unique_ptr<FileSystemAssetBrowserModelImplementation> impl_;
};

#endif // ASSET_BROWSER_MODEL_H_