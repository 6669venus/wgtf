#ifndef ASSET_BROWSER_VIEW_HPP
#define ASSET_BROWSER_VIEW_HPP

#include "core_reflection/object_handle.hpp"
#include "core_ui_framework/i_view.hpp"
#include "interfaces/panel_manager/i_panel_manager.hpp"
#include "core_dependency_system/i_interface.hpp"

namespace wgt
{
class PanelManager : public Implements<IPanelManager>
{
public:

	PanelManager( IComponentContext & contextManager );
	virtual ~PanelManager();

	virtual std::unique_ptr< IView > createAssetBrowser(
		ObjectHandleT<IAssetBrowserModel> dataModel,
		std::unique_ptr<IAssetBrowserEventModel> eventModel = nullptr);

private:
	IComponentContext& contextManager_;
	std::vector< IInterface * > types_;
};

} // end namespace wgt
#endif // ASSET_BROWSER_VIEW_HPP
