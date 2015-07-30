#ifndef I_ASSET_BROWSER_MODEL_HPP
#define I_ASSET_BROWSER_MODEL_HPP

#include "reflection/reflected_object.hpp"

class IComponentContext;
class IAssetListener;
class IFileSystem;

//------------------------------------------------------------------------------
// IAssetBrowserModel
//
// Represents the data model for the WGAssetBrowser control. Developers must
// implement their own versions depending on the type of system their tools
// use to access assets be it a loose file or resource pack file system. The
// data model does depend on the presence of IFileSystem in order to interact
// with the assets.
//------------------------------------------------------------------------------

class IAssetBrowserModel
{
	DECLARE_REFLECTED

public:

	//-------------------------------------
	// Lifecycle
	//-------------------------------------

	IAssetBrowserModel() : tempSizeT_(0), tempInt_(0)
	{
		// Just a temporary implementation until type definition registration
		// allows abstract classes.
	}

	virtual ~IAssetBrowserModel() {};
		
	//-------------------------------------
	// Public Methods
	//-------------------------------------

	// Initializes the data model. Expectation: Locate the IFileSystem
	// registered to the context manager.
	virtual void initialise( IComponentContext& contextManager,
		const std::string& assetPath ) {};

	// Add a listener for asset usage. Any registered listeners will be
	// notified when applyAsset is invoked.
	virtual void addListener( IAssetListener* listener ) {};
	
	// Asset path accessor/mutator
	virtual const std::vector<std::string>& assetPaths() const { return tempStrVector_; }

	// Populates the folderContents list with file information
	virtual void populateFolderContents( const std::vector<std::string>& paths ) {};

	// Accessor for the IFileSystem
	virtual IFileSystem* fileSystem() const { return nullptr; }

private:
	
	//-------------------------------------
	// Data Model Accessors
	//-------------------------------------

	// Retrieve the breadcrumbs
	// Expected: IListModel
	virtual ObjectHandle getBreadcrumbs() const { return ObjectHandle(); }

	// Retrieves the contents of the selected folder
	// Expected: IListModel
	virtual ObjectHandle getFolderContents() const { return ObjectHandle(); }

	// Retrieves the model for the folder tree view
	// Expected: ITreeModel
	virtual ObjectHandle getFolderTreeModel() const { return ObjectHandle(); }
	
	// Handlers for moving through the navigation history
	virtual bool navigateHistoryForward() const { return true; }
	virtual bool navigateHistoryBackward() const { return true; }

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

	// These are temporary variables. Type/definition registration does not 
	// allow for the registration of abstract classes. We need temporary
	// return values for the default implementation.
	std::vector<std::string> tempStrVector_;
	size_t tempSizeT_;
	int tempInt_;
};

#endif // I_ASSET_BROWSER_MODEL_HPP