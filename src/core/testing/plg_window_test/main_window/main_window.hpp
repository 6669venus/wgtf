#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "core_ui_framework/i_window.hpp"
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

	void close();
	void onCloseEvent( const IWindow* sender,
		const IWindow::CloseEventArgs& args );

	void addMenuBar( IUIApplication & uiApplication );

private:
	IUIApplication*			   app_;
	std::unique_ptr< IWindow > mainWindow_;
	std::unique_ptr< IAction > testExit_;
};


#endif // MAINWINDOW_H
