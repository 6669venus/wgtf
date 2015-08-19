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

	virtual void initialise( IComponentContext & context ) = 0;
	virtual std::unique_ptr<IView> createAssetBrowser(
		std::unique_ptr<IAssetBrowserModel> dataModel,
		ObjectHandle contextMenu = nullptr,
		std::unique_ptr<IAssetBrowserEventModel> eventModel = nullptr) = 0;
};

#endif // I_ASSET_BROWSER_MANAGER_HPP
