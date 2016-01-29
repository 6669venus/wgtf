#ifndef I_ASSET_BROWSER_MANAGER_HPP
#define I_ASSET_BROWSER_MANAGER_HPP

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
		std::unique_ptr<IAssetBrowserEventModel> eventModel = nullptr) = 0;
};

#endif // I_ASSET_BROWSER_MANAGER_HPP
