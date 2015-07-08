#ifndef TEST_ASSET_BROWSER_MODEL_HPP
#define TEST_ASSET_BROWSER_MODEL_HPP

#include "reflection/reflected_object.hpp"
#include "reflection/object_handle.hpp"
#include "data_model/asset_browser/i_asset_browser_model.hpp"

#include <memory>

class IContextManager;
class IAssetListener;
class IFileSystem;

//------------------------------------------------------------------------------

class TestAssetBrowserModel : public IAssetBrowserModel
{
public:
	
	//
	// IAssetBrowserModel Implementation
	//

	TestAssetBrowserModel();
	TestAssetBrowserModel( const TestAssetBrowserModel& rhs );

	virtual ~TestAssetBrowserModel() override; 
		
	virtual void initialise( IContextManager& contextManager, 
		const std::string& assetPath ) override;

	virtual void addListener( IAssetListener* listener ) override;

	virtual void populateFolderContents( const std::vector<std::string>& paths ) override;

	virtual const int & currentSelectedAssetIndex() const override;
	virtual void currentSelectedAssetIndex( const int & index ) override;

	virtual const std::vector<std::string>& assetPaths() const override;

	virtual bool useSelectedAsset() const override;

	virtual IFileSystem* fileSystem() const override;

	virtual bool navigateHistoryForward() const override;
	virtual bool navigateHistoryBackward() const override;

	virtual Variant getFolderTreeItemSelected() const override;
	virtual void setFolderTreeItemSelected( const Variant& selectedItem ) override;

	virtual ObjectHandle currentBreadcrumbItemIndex() const override;

	virtual const size_t & getCurrentBreadcrumbItemIndex() const override;
	virtual void setCurrentBreadcrumbItemIndex( const size_t & index ) override;

	virtual size_t getFolderTreeItemIndex() const override;

private:

	//
	// IAssetBrowserModel Implementation
	//

	virtual ObjectHandle getBreadcrumbs() const override;

	virtual ObjectHandle getFolderContents() const override;

	virtual ObjectHandle getFolderTreeModel() const override;

	//
	// Functions unique to TestAssetBrowserModel
	//

	void addFolderItems( const std::vector<std::string>& paths );

	const char* getThumbnail( const char * filename ) const;

	bool addAssetPath(const std::string& path);

private:

	//
	// Private Implementation Principle
	//

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

//------------------------------------------------------------------------------

class TestPageModel
{
	DECLARE_REFLECTED

public:

	TestPageModel();
	TestPageModel( const TestPageModel& rhs );

	virtual ~TestPageModel();

	void init( IContextManager & contextManager );

	ObjectHandle testModel() const;

private:
	ObjectHandle testModel_;
	IDefinitionManager* defManager_;
};

#endif // TEST_ASSET_BROWSER_MODEL_HPP