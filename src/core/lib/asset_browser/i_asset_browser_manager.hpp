#ifndef I_ASSET_BROWSER_MANAGER_HPP
#define I_ASSET_BROWSER_MANAGER_HPP

class IAssetListener;
class IContextManager;

class IAssetBrowserManager
{
public:

	IAssetBrowserManager() {}
	virtual ~IAssetBrowserManager() {}

	virtual void initialise( IContextManager & contextManager ) = 0;
	virtual void registerListener( IAssetListener* listener ) = 0;
	virtual void createView( IContextManager & contextManager ) = 0;
};

#endif // I_ASSET_BROWSER_MANAGER_HPP