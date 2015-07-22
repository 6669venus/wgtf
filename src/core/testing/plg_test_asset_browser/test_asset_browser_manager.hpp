#ifndef ASSET_BROWSER_VIEW_HPP
#define ASSET_BROWSER_VIEW_HPP

#include "reflection/object_handle.hpp"
#include "ui_framework/i_view.hpp"
#include "asset_browser/i_asset_browser_manager.hpp"
#include "dependency_system/i_interface.hpp"

class IContextManager;
class QQuickView;
class QObject;

class TestAssetBrowserManager : public Implements<IAssetBrowserManager>
{
public:

	TestAssetBrowserManager( IContextManager & contextManager );
	virtual ~TestAssetBrowserManager() {}

	virtual void initialise( IContextManager & contextManager );

	virtual void registerListener( IAssetListener* listener );

	virtual void createView( IContextManager & contextManager );

private:
	
	ObjectHandle pageModel_;
	std::shared_ptr<IView> assetBrowserView_;
	bool initialized_;
};

#endif // ASSET_BROWSER_VIEW_HPP