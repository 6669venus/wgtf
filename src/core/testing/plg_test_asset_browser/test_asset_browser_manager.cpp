#include "test_asset_browser_manager.hpp"
#include "generic_plugin/interfaces/i_context_manager.hpp"
#include "reflection/type_class_definition.hpp"
#include "ui_framework/i_ui_application.hpp"
#include "qt_common/i_qt_framework.hpp"
#include "data_model/asset_browser/i_asset_browser_model.hpp"
#include "data_model/asset_browser/asset_browser_view_model.hpp"
#include "data_model/asset_browser/asset_browser_event_model.hpp"

#include "data_model/asset_browser/file_system_asset_browser_model.hpp"

#include <QQuickView>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QObject>

TestAssetBrowserManager::TestAssetBrowserManager( 
	IContextManager & contextManager)
	: contextManager_(contextManager)
{
}

void TestAssetBrowserManager::initialise( IContextManager & contextManager )
{
	auto uiApplication = contextManager_.queryInterface< IUIApplication >();
	auto fileSystem = contextManager.queryInterface<IFileSystem>();
	auto definitionManager = contextManager_.queryInterface<IDefinitionManager>();
	assert(uiApplication != nullptr);
	assert(fileSystem != nullptr);
	assert(definitionManager != nullptr);
	if(!fileSystem || !definitionManager || !uiApplication)
		return;

	std::vector<std::string> assetPaths;
	assetPaths.emplace_back("../../../../../game/res");
	auto browserModel = std::unique_ptr<IAssetBrowserModel>(
		new FileSystemAssetBrowserModel(assetPaths, *fileSystem, *definitionManager));
	
	assetBrowserView_ = createAssetBrowser( std::move(browserModel) );
	if(assetBrowserView_)
	{
		uiApplication->addView(*assetBrowserView_);
	}
}

std::unique_ptr<IView> TestAssetBrowserManager::createAssetBrowser(
	std::unique_ptr<IAssetBrowserModel> dataModel,
	std::unique_ptr<IAssetBrowserEventModel> eventModel)
{
	if( !dataModel )
		return nullptr;

	if ( !eventModel )
		eventModel.reset(new AssetBrowserEventModel());

	auto uiFramework = contextManager_.queryInterface<IUIFramework>();
	auto definitionManager = contextManager_.queryInterface<IDefinitionManager>();
	assert(uiFramework != nullptr);
	assert(definitionManager != nullptr);
	
	auto viewDef = definitionManager->getDefinition<IAssetBrowserViewModel>();
	auto dataDef = definitionManager->getDefinition<IAssetBrowserModel>();
	auto eventDef = definitionManager->getDefinition<IAssetBrowserEventModel>();
	if ( viewDef && dataDef && eventDef )
	{
		auto viewModel = std::unique_ptr<IAssetBrowserViewModel>(new AssetBrowserViewModel(
			ObjectHandle(std::move(dataModel), dataDef),
			ObjectHandle(std::move(eventModel), eventDef)));

		return uiFramework->createView("qrc:///default/test_asset_browser_main.qml",
			IUIFramework::ResourceType::Url, ObjectHandle(std::move(viewModel), viewDef));
	}
	return nullptr;
}