#ifndef ASSET_BROWSER_VIEW_HPP
#define ASSET_BROWSER_VIEW_HPP

#include "core_reflection/object_handle.hpp"
#include "core_ui_framework/i_view.hpp"
#include "interfaces/panel_manager/i_panel_manager.hpp"
#include "core_dependency_system/i_interface.hpp"

class PanelManager : public Implements<IPanelManager>
{
public:

	PanelManager( IComponentContext & contextManager );
	virtual ~PanelManager();

	virtual void initialise( IComponentContext & contextManager );

	virtual std::unique_ptr<IView> createAssetBrowser(
		std::unique_ptr<IAssetBrowserModel> dataModel,
		ObjectHandleT<IAssetBrowserContextMenuModel> contextMenu = nullptr,
		std::unique_ptr<IAssetBrowserEventModel> eventModel = nullptr);

private:
	std::unique_ptr<IView> assetBrowserView_;
	IComponentContext& contextManager_;
	
	std::vector< IInterface * > types_;
};

#endif // ASSET_BROWSER_VIEW_HPP