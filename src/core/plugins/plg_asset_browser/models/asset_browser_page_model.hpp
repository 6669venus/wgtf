#ifndef ASSET_BROWSER_MODEL_HPP
#define ASSET_BROWSER_MODEL_HPP

#include "reflection/reflected_object.hpp"
#include "reflection/object_handle.hpp"

#include <memory>

class IContextManager;
class IAssetListener;
class IFileSystem;
//------------------------------------------------------------------------------
// AssetBrowserPageModel
//
// The main data model provided to the asset browser QML. Contains all of the
// individual data models required for the presentation.
//------------------------------------------------------------------------------

class AssetBrowserPageModel
{
public:

	AssetBrowserPageModel();
	AssetBrowserPageModel( const AssetBrowserPageModel& rhs );

	virtual ~AssetBrowserPageModel();
		
	void init( IContextManager& contextManager );

	void addListener( IAssetListener* listener );

	void populateFolderContents( const std::vector<std::string>& paths );


	const int & currentSelectedAssetIndex() const;
	void currentSelectedAssetIndex( const int & index );

	const std::vector<std::string>& assetPaths() const;
	bool addAssetPath(const std::string& path);

	ObjectHandle applyAsset() const;

	IFileSystem* fileSystem() const;

	ObjectHandle navigateHistoryForward() const;
	ObjectHandle navigateHistoryBackward() const;

	Variant getFolderTreeItemSelected() const;
	void setFolderTreeItemSelected( const Variant& selectedItem ) ;

private:
	DECLARE_REFLECTED

	void addFolderItems( const std::vector<std::string>& paths );

	ObjectHandle getBreadcrumbs() const;

	ObjectHandle getFolderContents() const;

	ObjectHandle getFolderTreeModel() const;

	const char* getThumbnail( const char * filename ) const;

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif // ASSET_BROWSER_MODEL_HPP