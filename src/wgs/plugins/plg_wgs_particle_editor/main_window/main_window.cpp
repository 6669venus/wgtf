#include "main_window.hpp"
#include "command_system/compound_command.hpp"
#include "reflection/interfaces/i_reflection_property_setter.hpp"

#include "data_model/reflection/reflected_tree_model.hpp"

#include "ui_framework/i_action.hpp"
#include "ui_framework/i_ui_application.hpp"
#include "ui_framework/i_ui_framework.hpp"
#include "ui_framework/i_view.hpp"
#include "ui_framework/i_window.hpp"

#include "generic_plugin/interfaces/i_context_manager.hpp"

#include "particle_node_tree_model.hpp"
#include "data_model/asset_browser/file_system_asset_browser_model.hpp"

#include <QFile>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>
#include <QString>
#include <QWidget>
#include <QDockWidget>
#include "qt_common/qml_view.hpp"

#include <QMainWindow>
#include "..\wgs\plugins\plg_wgs_core\i_viewport.hpp"

//==============================================================================
MainWindow::MainWindow()
{
}


//==============================================================================
MainWindow::~MainWindow()
{
}


//==============================================================================
void MainWindow::init(IContextManager& contextManager)
{
	auto uiApplication = contextManager.queryInterface<IUIApplication>();
	auto uiFramework = contextManager.queryInterface<IUIFramework>();

	if ( !uiApplication || !uiFramework )
		return;

	Q_INIT_RESOURCE(resources);

	uiFramework->loadActionData(":/plg_wgs_particle_editor/actions", IUIFramework::ResourceType::File);
	uiFramework->setPluginPath("plugins");	// Set the plugin path, so we can load custom plugins.

	mainWindow_ = uiFramework->createWindow(":/plg_wgs_particle_editor/mainwindow", IUIFramework::ResourceType::File);
	uiApplication->addWindow(*mainWindow_);
	mainWindow_->show();
	
	bindViewport(contextManager);

	createActions(*uiFramework, *uiApplication);
	createViews(contextManager, *uiFramework, *uiApplication);
}


//------------------------------------------------------------------------------
void MainWindow::fini()
{
	destroyViews();
	destroyActions();

	mainWindow_.reset();

	Q_CLEANUP_RESOURCE(resources);
}

void MainWindow::createActions(IUIFramework& uiFramework, IUIApplication& uiApplication)
{
	actions_.emplace_back(uiFramework.createAction("New", std::bind(&MainWindow::actionNew, this)));
	actions_.emplace_back(uiFramework.createAction("Open", std::bind(&MainWindow::actionNone, this)));
	actions_.emplace_back(uiFramework.createAction("Save", std::bind(&MainWindow::actionNone, this)));
	actions_.emplace_back(uiFramework.createAction("SaveAs", std::bind(&MainWindow::actionNone, this)));
	actions_.emplace_back(uiFramework.createAction("Preferences", std::bind(&MainWindow::actionNone, this)));
	actions_.emplace_back(uiFramework.createAction("Exit", std::bind(&MainWindow::actionClose, this)));

	actions_.emplace_back(uiFramework.createAction("Undo", std::bind(&MainWindow::actionNone, this)));
	actions_.emplace_back(uiFramework.createAction("Refresh", std::bind(&MainWindow::actionNone, this)));

	actions_.emplace_back(uiFramework.createAction("ViewToolbarNodes", std::bind(&MainWindow::actionNone, this)));
	actions_.emplace_back(uiFramework.createAction("ViewToolbarProperties", std::bind(&MainWindow::actionNone, this)));
	actions_.emplace_back(uiFramework.createAction("ViewToolbarCurve", std::bind(&MainWindow::actionNone, this)));
	actions_.emplace_back(uiFramework.createAction("ViewToolbarAssetBrowser", std::bind(&MainWindow::actionNone, this)));

	actions_.emplace_back(uiFramework.createAction("HelpAbout", std::bind(&MainWindow::actionNone, this)));


	for( auto itr = actions_.cbegin(); itr != actions_.cend(); ++itr )
	{
		uiApplication.addAction(**itr);
	}
}


void MainWindow::destroyActions()
{
	for( auto itr = actions_.begin(); itr != actions_.end(); ++itr )
	{
		(*itr).reset();
	}
}

#include "data_model/asset_browser/asset_browser_view_model.hpp"
#include "data_model/asset_browser/asset_browser_event_model.hpp"
std::unique_ptr< IView > CreateAssetBrowser(IContextManager& contextManager,
	IUIFramework& uiFramework,
	IDefinitionManager* definitionManager,
	std::unique_ptr<IAssetBrowserModel> dataModel,
	std::unique_ptr<IAssetBrowserEventModel> eventModel = nullptr)
{
	if(!dataModel)
		return nullptr;

	if(!eventModel)
		eventModel.reset( new AssetBrowserEventModel() );

	dataModel->initialise(contextManager);
	
	auto viewDef = definitionManager->getDefinition<IAssetBrowserViewModel>();
	auto dataDef = definitionManager->getDefinition<IAssetBrowserModel>();
	auto eventDef = definitionManager->getDefinition<IAssetBrowserEventModel>();
	if ( viewDef && dataDef && eventDef )
	{
		auto viewModel = std::unique_ptr<IAssetBrowserViewModel>( new AssetBrowserViewModel(
			ObjectHandle(std::move(dataModel), dataDef),
			ObjectHandle(std::move(eventModel), eventDef) ) );

		return uiFramework.createView("qrc:///plg_wgs_particle_editor/asset_browser",
			IUIFramework::ResourceType::Url, ObjectHandle(std::move(viewModel), viewDef));
	}
	return nullptr;
}

void MainWindow::createViews(IContextManager& contextManager, IUIFramework& uiFramework, IUIApplication& uiApplication)
{
	// Disabling the viewport qml. Will likely need to achieve this a different way.
	//panels_.emplace_back(uiFramework.createView("qrc:///plg_wgs_particle_editor/viewport", IUIFramework::ResourceType::Url));

	panels_.emplace_back(uiFramework.createView("qrc:///plg_wgs_particle_editor/properties_editor", IUIFramework::ResourceType::Url));

	// Create the default tree view model
	auto model = std::unique_ptr<ITreeModel>(new ParticleNodeTreeModel());
	panels_.emplace_back(uiFramework.createView("qrc:///plg_wgs_particle_editor/tree_editor", IUIFramework::ResourceType::Url, std::move(model)));

	std::vector<std::string> assetPaths;
	assetPaths.push_back("\\");
	auto fileSystem = contextManager.queryInterface<IFileSystem>();
	auto definitionManager = contextManager.queryInterface<IDefinitionManager>();
	if (fileSystem && definitionManager)
	{
		auto dataModel = std::unique_ptr<FileSystemAssetBrowserModel>(
			new FileSystemAssetBrowserModel(assetPaths, *fileSystem, *definitionManager) );
		auto assetBrowser = CreateAssetBrowser(contextManager, uiFramework, definitionManager, std::move(dataModel));
		panels_.emplace_back(std::move(assetBrowser));
	}

	for (auto itr = panels_.cbegin(); itr != panels_.cend(); ++itr)
	{
		uiApplication.addView(**itr);
	}
}


void MainWindow::destroyViews()
{
	for( auto itr = panels_.begin(); itr != panels_.end(); ++itr )
	{
		(*itr).reset();
	}
}

void MainWindow::bindViewport(IContextManager& contextManager)
{
	auto qmlWindow = dynamic_cast<QtWindow*>(mainWindow_.get());
	if( qmlWindow != nullptr )
	{
		QMainWindow* mainWindow = qmlWindow->window();
		if( mainWindow )
		{
			mainWindow->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);

			QWidget* viewportPlugin = mainWindow->findChild<QWidget*>("viewport");

			if( viewportPlugin != nullptr )
			{
				WId wid = viewportPlugin->winId();

				// Refresh the renderer with the correct hwnd.
				auto& viewport = *contextManager.queryInterface<IViewport>();
				viewport.SetGraphicsEnvironment(wid);
			}
		}
	}
}



void MainWindow::actionClose()
{
	mainWindow_->close();
}

void MainWindow::actionNew()
{
}


void MainWindow::actionNone()
{
}