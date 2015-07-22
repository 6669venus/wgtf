#include "test_asset_browser_manager.hpp"
#include "generic_plugin/interfaces/i_context_manager.hpp"
#include "reflection/type_class_definition.hpp"
#include "ui_framework/i_ui_application.hpp"
#include "qt_common/i_qt_framework.hpp"
#include "data_model/asset_browser/file_system_asset_browser_model.hpp"

#include <QQuickView>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QObject>

TestAssetBrowserManager::TestAssetBrowserManager( 
	IContextManager & contextManager)
	: pageModel_(nullptr)
	, initialized_( false )
{
}

void TestAssetBrowserManager::initialise( IContextManager & contextManager )
{	
	if (!initialized_)
	{
		// Create the view
		// TODO: We will want to make this a manual request via the interface
		//       as we iterate and exit prototyping.
		createView( contextManager );

		// Flag initialization as complete
		initialized_ = true;
	}
}

void TestAssetBrowserManager::createView( IContextManager & contextManager )
{
	// Setup the display via QML with the model as input
	auto uiApplication = contextManager.queryInterface< IUIApplication >();
	assert( uiApplication != nullptr );
	
	IUIFramework* qtFramework = contextManager.queryInterface<IUIFramework>();
	assert( qtFramework != nullptr );


	// Setup the model for the view
	auto defManager = contextManager.queryInterface<IDefinitionManager>();
	assert(defManager != nullptr);

	auto def = defManager->getDefinition< IAssetBrowserModel >();
	assert(def != nullptr);

	auto& fileSystem = *contextManager.queryInterface<IFileSystem>();
	assert(&fileSystem != nullptr);

	std::vector<std::string> assetPaths;
	assetPaths.emplace_back("../../../../../game/res");
	auto browserModel = std::unique_ptr<IAssetBrowserModel>(
		new FileSystemAssetBrowserModel(assetPaths, fileSystem, *defManager ) );
	pageModel_ = ObjectHandle(std::move(browserModel), def);

	assetBrowserView_ = qtFramework->createView(
		"qrc:///default/test_asset_browser_main.qml",
		IUIFramework::ResourceType::Url, pageModel_ );
	uiApplication->addView( *assetBrowserView_ );
}

void TestAssetBrowserManager::registerListener( IAssetListener* listener )
{
	pageModel_.getBase< IAssetBrowserModel >()->addListener( listener );
}
