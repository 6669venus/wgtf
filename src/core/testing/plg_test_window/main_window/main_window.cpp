#include "main_window.hpp"
#include "command_system/i_command_manager.hpp"
#include "command_system/compound_command.hpp"
#include "reflection/interfaces/i_reflection_property_setter.hpp"

#include "data_model/reflection/reflected_tree_model.hpp"

#include "ui_framework/i_action.hpp"
#include "ui_framework/i_ui_application.hpp"
#include "ui_framework/i_ui_framework.hpp"
#include "ui_framework/i_view.hpp"
#include "ui_framework/i_window.hpp"

//==============================================================================
MainWindow::MainWindow()
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
}

//------------------------------------------------------------------------------
void MainWindow::fini()
{
	destroyActions();

	mainWindow_.reset();
}

void MainWindow::close()
{
	mainWindow_->close();
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