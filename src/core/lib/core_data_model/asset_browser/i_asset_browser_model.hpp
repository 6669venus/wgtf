#ifndef I_ASSET_BROWSER_MODEL_HPP
#define I_ASSET_BROWSER_MODEL_HPP

#include "core_reflection/reflected_object.hpp"

class IComponentContext;
class IItem;

typedef std::vector<std::string> AssetPaths;

//------------------------------------------------------------------------------
// IAssetBrowserModel
//
// Represents the data model for the WGAssetBrowser control. Developers must
// implement their own versions depending on the type of system their tools
// use to access assets be it a loose file or resource pack file system.
//------------------------------------------------------------------------------
class IAssetBrowserModel
{
	DECLARE_REFLECTED

public:

	//-------------------------------------
	// Lifecycle
	//-------------------------------------
	virtual ~IAssetBrowserModel() {};
		
	//-------------------------------------
	// Public Methods
	//-------------------------------------

	// Initializes the data model.
	virtual void initialise( IComponentContext& contextManager ) {}

	// Populate the folderContents list given the specified item
	virtual void populateFolderContents( const IItem* item ) {}
	
	// Asset path accessor/mutator
	virtual const AssetPaths& assetPaths() const
	{
		assert(!"must override IAssetBrowserModel::assetPaths() method");
		return tempStrVector_;
	}

	// Retrieves the contents of the selected folder
	// Expected: IListModel
	virtual ObjectHandle getFolderContents() const { return ObjectHandle(); }

	// Retrieves the model for the folder tree view
	// Expected: ITreeModel
	virtual ObjectHandle getFolderTreeModel() const { return ObjectHandle(); }
	
private:

	// These are temporary variables. Type/definition registration does not 
	// allow for the registration of abstract classes. We need temporary
	// return values for the default implementation.
	std::vector<std::string> tempStrVector_;
};

#endif // I_ASSET_BROWSER_MODEL_HPP