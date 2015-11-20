#include "panel_manager.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/type_class_definition.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_data_model/asset_browser/i_asset_browser_model.hpp"
#include "core_data_model/asset_browser/i_asset_browser_context_menu_model.hpp"
#include "core_data_model/asset_browser/asset_browser_view_model.hpp"
#include "core_data_model/asset_browser/asset_browser_event_model.hpp"

#include "core_data_model/asset_browser/file_system_asset_browser_model.hpp"

#include <QQuickView>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QObject>

PanelManager::PanelManager( IComponentContext & contextManager )
	: contextManager_( contextManager )
{
}

PanelManager::~PanelManager()
{	
	for ( auto type : types_ )
	{
		contextManager_.deregisterInterface(type);
	}
}

void PanelManager::initialise( IComponentContext & contextManager )
{
}

void PanelManager::finalise()
{
	auto uiApplication = contextManager_.queryInterface< IUIApplication >();
	assert( uiApplication != nullptr );

	for (auto& view : assetBrowserViews_)
	{
		uiApplication->removeView( *view.get() );
		view.reset();
	}

	tAssetBrowserDataModels::iterator itrModel = assetBrowserDataModels_.begin();
	tAssetBrowserDataModels::iterator itrModelsEnd = assetBrowserDataModels_.end();
	for (auto& model : assetBrowserDataModels_)
	{
		model->finalise();
		model = nullptr;
	}
}
	
std::weak_ptr< IView > PanelManager::createAssetBrowser(
	std::unique_ptr<IAssetBrowserModel> dataModel,
	ObjectHandleT<IAssetBrowserContextMenuModel> contextMenu,
	std::unique_ptr<IAssetBrowserEventModel> eventModel)
{
	if( !dataModel )
	{
		return std::weak_ptr< IView >();
	}

	// The variant meta type manager is required for converting an IAssetObjectModel
	if(Variant::getMetaTypeManager() == nullptr)
	{
		Variant::setMetaTypeManager( Context::queryInterface< IMetaTypeManager >() );
	}

	if ( !eventModel )
	{
		eventModel.reset(new AssetBrowserEventModel());
	}

	auto uiApplication = contextManager_.queryInterface< IUIApplication >();
	auto uiFramework = contextManager_.queryInterface<IUIFramework>();
	auto definitionManager = contextManager_.queryInterface<IDefinitionManager>();
	assert( uiApplication != nullptr );
	assert(uiFramework != nullptr);
	assert(definitionManager != nullptr);
	
	auto viewDef = definitionManager->getDefinition<IAssetBrowserViewModel>();
	auto dataDef = definitionManager->getDefinition<IAssetBrowserModel>();
	auto eventDef = definitionManager->getDefinition<IAssetBrowserEventModel>();
	if ( viewDef && dataDef && eventDef )
	{
		dataModel->initialise( contextManager_, *definitionManager );
		types_.emplace_back(contextManager_.registerInterface(eventModel.get(), false));
		auto assetBrowserDataModel = ObjectHandleT<IAssetBrowserModel>(std::move(dataModel), dataDef);
		auto assetBrowserEventModel = ObjectHandleT<IAssetBrowserEventModel>(std::move(eventModel), eventDef);
		auto viewModel = std::unique_ptr<IAssetBrowserViewModel>(new AssetBrowserViewModel(	*definitionManager, assetBrowserDataModel, std::move(contextMenu), assetBrowserEventModel));

		auto contextMenuModel = viewModel->contextMenu().getBase< IAssetBrowserContextMenuModel >();
		if (contextMenuModel != nullptr)
		{
			contextMenuModel->setViewModel( viewModel.get() );
		}

		assetBrowserDataModels_.emplace_back( assetBrowserDataModel );
				
		auto assetBrowserView = uiFramework->createView("plg_panel_manager/asset_browser_panel.qml",
			IUIFramework::ResourceType::Url, ObjectHandle(std::move(viewModel), viewDef));

		std::shared_ptr< IView > sharedView( std::move( assetBrowserView ) );
		assetBrowserViews_.emplace_back( sharedView );
		return sharedView;
	}
	
	return std::weak_ptr< IView >();
}
