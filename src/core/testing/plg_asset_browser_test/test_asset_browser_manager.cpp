#include "test_asset_browser_manager.hpp"
#include "metadata/test_asset_browser_models.mpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/type_class_definition.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_qt_common/i_qt_framework.hpp"

#include <QQuickView>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QObject>

TestAssetBrowserManager::TestAssetBrowserManager( 
	IComponentContext & contextManager )
	: pageModel_( nullptr )
	, initialized_( false )
{
	// Setup the model for the view
	auto defManager = 
		contextManager.queryInterface<IDefinitionManager>();
	assert( defManager != nullptr );

	defManager->registerDefinition( new TypeClassDefinition< 
		TestPageModel >() );
			
	auto def = defManager->getDefinition< TestPageModel >();
	auto impl = std::unique_ptr< TestPageModel >( new TestPageModel );
	pageModel_ = ObjectHandle( std::move( impl ), def );
}

void TestAssetBrowserManager::initialise( IComponentContext & contextManager )
{	
	if (!initialized_)
	{
		auto defManager = 
			contextManager.queryInterface<IDefinitionManager>();
		assert( defManager != nullptr );

		pageModel_.getBase< TestPageModel >()->init( contextManager );

		// Create the view
		// TODO: We will want to make this a manual request via the interface
		//       as we iterate and exit prototyping.
		createView( contextManager );

		// Flag initialization as complete
		initialized_ = true;
	}
}

void TestAssetBrowserManager::createView( IComponentContext & contextManager )
{
	// Setup the display via QML with the model as input
	auto uiApplication = contextManager.queryInterface< IUIApplication >();
	assert( uiApplication != nullptr );
	
	IUIFramework* qtFramework = contextManager.queryInterface<IUIFramework>();
	assert( qtFramework != nullptr );

	assetBrowserView_ = qtFramework->createView(
		"qrc:///default/test_asset_browser_main.qml",
		IUIFramework::ResourceType::Url, pageModel_ );
	uiApplication->addView( *assetBrowserView_ );
}

void TestAssetBrowserManager::registerListener( IAssetListener* listener )
{
	pageModel_.getBase< TestPageModel >()->testModel().getBase< 
		IAssetBrowserModel >()->addListener( listener );
}
