#ifndef I_ASSET_BROWSER_MANAGER_HPP
#define I_ASSET_BROWSER_MANAGER_HPP

class IAssetBrowserModel;
class IAssetBrowserEventModel;
class IContextManager;

class IAssetBrowserManager
{
public:

	IAssetBrowserManager() {}
	virtual ~IAssetBrowserManager() {}

	virtual void initialise( IContextManager & contextManager ) = 0;
	virtual std::unique_ptr<IView> createAssetBrowser(
		std::unique_ptr<IAssetBrowserModel> dataModel,
		std::unique_ptr<IAssetBrowserEventModel> eventModel = nullptr) = 0;
};

#endif // I_ASSET_BROWSER_MANAGER_HPP