#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

class IAction;
class IUIApplication;
class IComponentContext;

class MainWindow
{    
public:
    explicit MainWindow();
    ~MainWindow();

	void init( IUIApplication * uiApplication, IComponentContext & contextManager );
	void fini();

private:
	std::unique_ptr< IAction > testCommand1_;
	std::unique_ptr< IAction > testCommand2_;
	IComponentContext * contextManager_;

	void createToolBar();
	void createMenuBar();
	void createActions();
	void executeTestCommand1();
	void executeTestCommand2();
};


#endif // MAINWINDOW_H
