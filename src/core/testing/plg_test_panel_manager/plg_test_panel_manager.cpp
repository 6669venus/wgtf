#include "generic_plugin/generic_plugin.hpp"
#include "ui_framework/i_ui_application.hpp"
#include "ui_framework/i_view.hpp"
#include "reflection/i_definition_manager.hpp"

#include "serialization/interfaces/i_file_system.hpp"
#include "data_model/asset_browser/file_system_asset_browser_model.hpp"
#include "data_model/asset_browser/i_asset_browser_event_model.hpp"
#include "interfaces/panel_manager/i_panel_manager.hpp"

//==============================================================================
class TestPanelManagerPlugin
	: public PluginMain
{
public:
	//==========================================================================
	TestPanelManagerPlugin(IComponentContext & contextManager) {}

	virtual void Initialise(IComponentContext& contextManager)
	{
		Variant::setMetaTypeManager( contextManager.queryInterface< IMetaTypeManager >() );

		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		auto definitionManager = contextManager.queryInterface<IDefinitionManager>();
		auto fileSystem = contextManager.queryInterface<IFileSystem>();
		auto panelManager = contextManager.queryInterface<IPanelManager>();
		assert(uiApplication != nullptr);
		assert(fileSystem != nullptr);
		assert(definitionManager != nullptr);
		assert(panelManager != nullptr);
		if(!fileSystem || !definitionManager || !uiApplication || !panelManager)
			return;

		std::vector<std::string> assetPaths;
		assetPaths.emplace_back("../../../../../game/res");
		auto browserModel = std::unique_ptr<IAssetBrowserModel>(
			new FileSystemAssetBrowserModel(assetPaths, *fileSystem, *definitionManager));
		
		assetBrowserView_ = panelManager->createAssetBrowser( std::move(browserModel) );
		if(assetBrowserView_)
		{
			uiApplication->addView(*assetBrowserView_);
		}
	}

private:
	std::unique_ptr<IView> assetBrowserView_;
};

PLG_CALLBACK_FUNC(TestPanelManagerPlugin)
