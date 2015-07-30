#ifndef ASSET_BROWSER_VIEW_HPP
#define ASSET_BROWSER_VIEW_HPP

#include "reflection/object_handle.hpp"
#include "ui_framework/i_view.hpp"
#include "models/test_asset_browser_models.hpp"
#include "asset_browser/i_asset_browser_manager.hpp"

class IComponentContext;
class QQuickView;
class QObject;

class TestAssetBrowserManager : public Implements<IAssetBrowserManager>
{
public:

	TestAssetBrowserManager( IComponentContext & contextManager );
	virtual ~TestAssetBrowserManager() {}

	virtual void initialise( IComponentContext & contextManager );

	virtual void registerListener( IAssetListener* listener );

	virtual void createView( IComponentContext & contextManager );

private:
	
	ObjectHandle pageModel_;
	std::shared_ptr<IView> assetBrowserView_;
	bool initialized_;
};

#endif // ASSET_BROWSER_VIEW_HPP