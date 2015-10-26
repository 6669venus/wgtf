#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "core_ui_framework/i_window.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include <memory>

class IAction;
class IUIApplication;
class IUIFramework;

class MainWindow
{    
public:
    explicit MainWindow();
    ~MainWindow();

	void init( IUIApplication & uiApplication, IUIFramework & uiFramework );
	void fini();

private:
	void createActions( IUIFramework & uiFramework );
	void destroyActions();

	void close( IAction * action );
	void onCloseEvent( const IWindow* sender,
		const IWindow::CloseEventArgs& args );

	void onStartUp( const IApplication * sender, const IApplication::StartUpArgs & args );

	void addMenuBar( IUIApplication & uiApplication );

private:
	IUIApplication*			   app_;
	std::unique_ptr< IWindow > mainWindow_;
	std::unique_ptr< IAction > testExit_;
};


#endif // MAINWINDOW_H
