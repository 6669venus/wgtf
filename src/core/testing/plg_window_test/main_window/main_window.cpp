#include "main_window.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_command_system/compound_command.hpp"
#include "core_reflection/interfaces/i_reflection_property_setter.hpp"

#include "core_data_model/reflection/reflected_tree_model.hpp"

#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_window.hpp"

//==============================================================================
MainWindow::MainWindow()
	: app_( nullptr )
{
}

//==============================================================================
MainWindow::~MainWindow()
{

}

//==============================================================================
void MainWindow::init( IUIApplication & uiApplication, IUIFramework & uiFramework )
{
	uiApplication.onStartUp().add< MainWindow, &MainWindow::onStartUp >( this );
	uiFramework.loadActionData( 
		":/plg_window_test/actions.xml", IUIFramework::ResourceType::File );
	mainWindow_ = uiFramework.createWindow( 
		":/plg_window_test/main_window.ui", IUIFramework::ResourceType::File );
	uiApplication.addWindow( *mainWindow_ );

	createActions( uiFramework );
	addMenuBar( uiApplication );
	app_ = &uiApplication;
	mainWindow_->onCloseEvent().add< MainWindow, &MainWindow::onCloseEvent >( this );
}

//------------------------------------------------------------------------------
void MainWindow::fini()
{
	mainWindow_->onCloseEvent().remove< MainWindow, &MainWindow::onCloseEvent >( this );
	app_->removeAction( *testExit_ );
	app_->removeWindow( *mainWindow_ );
	destroyActions();
	mainWindow_.reset();
	app_->onStartUp().remove< MainWindow, &MainWindow::onStartUp >( this );
}

void MainWindow::close( IAction * action )
{
	mainWindow_->close();
}

void MainWindow::onCloseEvent( const IWindow* sender,
							  const IWindow::CloseEventArgs& args )
{
	assert( app_ != nullptr );
	app_->quitApplication();
}

void MainWindow::createActions( IUIFramework & uiFramework )
{
	// hook application exit
	testExit_ = uiFramework.createAction(
		"Exit", 
		std::bind( &MainWindow::close, this, std::placeholders::_1 ) );
}

void MainWindow::destroyActions()
{
	testExit_.reset();
}

void MainWindow::addMenuBar( IUIApplication & uiApplication )
{
	uiApplication.addAction( *testExit_ );
}

void MainWindow::onStartUp( const IApplication * sender, const IApplication::StartUpArgs & args )
{
	assert( app_ == sender );
	mainWindow_->showMaximized( true );
}
