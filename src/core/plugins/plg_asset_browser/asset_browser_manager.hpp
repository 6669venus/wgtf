#ifndef ASSET_BROWSER_VIEW_HPP
#define ASSET_BROWSER_VIEW_HPP

#include "reflection/object_handle.hpp"
#include "ui_framework/i_view.hpp"
#include "models/asset_browser_page_model.hpp"
#include "asset_browser/i_asset_browser_manager.hpp"

class IContextManager;
class QQuickView;
class QObject;

class AssetBrowserManager : public Implements<IAssetBrowserManager>
{
public:

	AssetBrowserManager( IContextManager & contextManager );
	virtual ~AssetBrowserManager() {}

	virtual void initialise( IContextManager & contextManager );

	virtual void registerListener( IAssetListener* listener );

	virtual void createView( IContextManager & contextManager );

private:
	
	ObjectHandle assetBrowserPageModel_;
	std::shared_ptr<IView> assetBrowserView_;
	bool initialized_;
};

#endif // ASSET_BROWSER_VIEW_HPP