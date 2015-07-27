#ifndef ASSET_BROWSER_VIEW_HPP
#define ASSET_BROWSER_VIEW_HPP

#include "reflection/object_handle.hpp"
#include "ui_framework/i_view.hpp"
#include "asset_browser/i_asset_browser_manager.hpp"
#include "dependency_system/i_interface.hpp"

class TestAssetBrowserManager : public Implements<IAssetBrowserManager>
{
public:

	TestAssetBrowserManager( IContextManager & contextManager );
	virtual ~TestAssetBrowserManager() {}

	virtual void initialise( IContextManager & contextManager );
	
	virtual std::unique_ptr<IView> createAssetBrowser(
		std::unique_ptr<IAssetBrowserModel> dataModel,
		std::unique_ptr<IAssetBrowserEventModel> eventModel = nullptr);

private:
	std::unique_ptr<IView> assetBrowserView_;
	IContextManager& contextManager_;
};

#endif // ASSET_BROWSER_VIEW_HPP