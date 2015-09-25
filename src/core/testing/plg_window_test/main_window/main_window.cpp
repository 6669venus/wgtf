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
	uiFramework.loadActionData( 
		":/testing/actiondata", IUIFramework::ResourceType::File );
	mainWindow_ = uiFramework.createWindow( 
		":/testing/mainwindow", IUIFramework::ResourceType::File );
	uiApplication.addWindow( *mainWindow_ );
	mainWindow_->show();

	createActions( uiFramework );
	addMenuBar( uiApplication );
	app_ = &uiApplication;
	mainWindow_->onCloseEvent().add< MainWindow, &MainWindow::onCloseEvent >( this );
}

//------------------------------------------------------------------------------
void MainWindow::fini()
{
	mainWindow_->onCloseEvent().remove< MainWindow, &MainWindow::onCloseEvent >( this );
	destroyActions();

	mainWindow_.reset();
}

void MainWindow::close()
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
		std::bind( &MainWindow::close, this ) );
}

void MainWindow::destroyActions()
{
	testExit_.reset();
}

void MainWindow::addMenuBar( IUIApplication & uiApplication )
{
	uiApplication.addAction( *testExit_ );
}