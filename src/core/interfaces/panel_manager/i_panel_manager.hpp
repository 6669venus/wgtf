#ifndef I_ASSET_BROWSER_MANAGER_HPP
#define I_ASSET_BROWSER_MANAGER_HPP

#include "core/lib/core_data_model/asset_browser/i_asset_browser_context_menu_model.hpp"

class IAssetBrowserModel;
class IAssetBrowserEventModel;
class IComponentContext;
class IView;

class IPanelManager
{
public:

	IPanelManager() {}
	virtual ~IPanelManager() {}

	virtual std::unique_ptr< IView > createAssetBrowser(
		ObjectHandleT<IAssetBrowserModel> dataModel,
		ObjectHandleT<IAssetBrowserContextMenuModel> contextMenu = nullptr,
		std::unique_ptr<IAssetBrowserEventModel> eventModel = nullptr) = 0;
};

#endif // I_ASSET_BROWSER_MANAGER_HPP
