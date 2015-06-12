#include "asset_browser_manager.hpp"
#include "metadata/asset_browser_models.mpp"
#include "generic_plugin/interfaces/i_context_manager.hpp"
#include "reflection/type_class_definition.hpp"
#include "ui_framework/i_ui_application.hpp"
#include "qt_common/i_qt_framework.hpp"

#include <QQuickView>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QObject>

AssetBrowserManager::AssetBrowserManager( IContextManager & contextManager )
	: assetBrowserPageModel_( nullptr )
	, initialized_( false )
{
		// Setup the model(s) for the view
		auto pDefinitionManager = 
			contextManager.queryInterface<IDefinitionManager>();
		assert( pDefinitionManager != nullptr );

		IDefinitionManager& definitionManager = (*pDefinitionManager);
		REGISTER_DEFINITION( AssetBrowserPageModel );

		auto definition = pDefinitionManager->getDefinition(
			getClassIdentifier< AssetBrowserPageModel >() );
		assert( definition != nullptr );
		assetBrowserPageModel_ = definition->create();
}

void AssetBrowserManager::initialise( IContextManager & contextManager )
{	
	if (!initialized_)
	{
		assetBrowserPageModel_.getBase< AssetBrowserPageModel >()->init( 
			contextManager );

		// Create the view
		// TODO: We will want to make this a manual request via the interface
		//       as we iterate and exit prototyping.
		createView( contextManager );

		// Flag initialization as complete
		initialized_ = true;
	}
}

void AssetBrowserManager::createView( IContextManager & contextManager )
{
	// Setup the display via QML with the model as input
	auto uiApplication = contextManager.queryInterface< IUIApplication >();
	assert( uiApplication != nullptr );
	
	IUIFramework* qtFramework = contextManager.queryInterface<IUIFramework>();
	assert( qtFramework != nullptr );

	assetBrowserView_ = qtFramework->createView(
		"qrc:///default/asset_browser_main.qml",
		IUIFramework::ResourceType::Url, assetBrowserPageModel_ );
	uiApplication->addView( *assetBrowserView_ );
}

void AssetBrowserManager::registerListener( IAssetListener* listener )
{
	assetBrowserPageModel_.getBase< AssetBrowserPageModel >()->addListener( 
		listener );
}
