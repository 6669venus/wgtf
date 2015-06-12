#ifndef ASSET_BROWSER_MODEL_HPP
#define ASSET_BROWSER_MODEL_HPP

#include "reflection/reflected_object.hpp"
#include "reflection/object_handle.hpp"

#include <memory>

class IContextManager;
class IAssetListener;

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

	void populateFolderContents( const char* assetFolderPath );

	void addFolderItem( const char* fileName, const char* thumbnail );

	const int & currentSelectedAssetIndex() const;
	void currentSelectedAssetIndex( const int & index );

	ObjectHandle applyAsset() const;

	ObjectHandle navigateHistoryForward() const;
	ObjectHandle navigateHistoryBackward() const;

private:

	DECLARE_REFLECTED

	ObjectHandle getBreadcrumbs() const;

	ObjectHandle getFolderContents() const;

	ObjectHandle getFolderTreeModel() const;

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif // ASSET_BROWSER_MODEL_HPP