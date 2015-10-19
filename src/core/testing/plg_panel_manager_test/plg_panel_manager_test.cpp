#include "test_asset_presentation_provider.hpp"

#include "core_generic_plugin/generic_plugin.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_reflection/i_definition_manager.hpp"

#include "core_serialization/interfaces/i_file_system.hpp"
#include "core_data_model/asset_browser/file_system_asset_browser_model.hpp"
#include "core_data_model/asset_browser/i_asset_browser_event_model.hpp"
#include "interfaces/panel_manager/i_panel_manager.hpp"

//==============================================================================
class TestPanelManagerPlugin
	: public PluginMain
{
public:
	//==========================================================================
	TestPanelManagerPlugin(IComponentContext & contextManager) {}

	void Initialise(IComponentContext& contextManager) override
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

		presentationProvider_.generateData();

		std::vector<std::string> assetPaths;
		std::vector<std::string> customFilters;
		assetPaths.emplace_back("../../");
		auto browserModel = std::unique_ptr<IAssetBrowserModel>(
			new FileSystemAssetBrowserModel(assetPaths, customFilters, *fileSystem, 
											*definitionManager, presentationProvider_));
		
		assetBrowserView_ = panelManager->createAssetBrowser( std::move(browserModel) );
		if(assetBrowserView_)
		{
			uiApplication->addView(*assetBrowserView_);
		}
	}

	bool Finalise( IComponentContext & contextManager ) override
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		assert( uiApplication != nullptr );
		uiApplication->removeView( *assetBrowserView_ );
		assetBrowserView_ = nullptr;
		return true;
	}

private:
	std::unique_ptr<IView> assetBrowserView_;
	TestAssetPresentationProvider presentationProvider_;
};

PLG_CALLBACK_FUNC(TestPanelManagerPlugin)
