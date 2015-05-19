#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

class IAction;
class IUIApplication;
class IUIFramework;
class IView;
class IWindow;

class MainWindow
{    
public:
    explicit MainWindow();
    ~MainWindow();

	void init( IUIApplication & uiApplication, IUIFramework & uiFramework );
	void fini();

private:
	void createActions();
	void destroyActions();

	void close();

	void addMenuBar( IUIApplication & uiApplication );

private:
	std::unique_ptr< IWindow > mainWindow_;
	std::unique_ptr< IAction > testExit_;
};


#endif // MAINWINDOW_H
