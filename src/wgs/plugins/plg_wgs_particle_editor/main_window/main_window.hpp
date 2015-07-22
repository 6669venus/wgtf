#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include "qt_common/qt_window.hpp"

class IAction;
class IUIApplication;
class IUIFramework;
class IView;
class IWindow;
class IContextManager;

class MainWindow
{
public:
	explicit MainWindow();
	~MainWindow();

	void init(IContextManager& contextManager);
	void fini();

private:
	void createActions(IUIFramework& uiFramework, IUIApplication& uiApplication);
	void destroyActions();

	void createViews(IContextManager& contextManager, IUIFramework& uiFramework, IUIApplication& uiApplication);
	void destroyViews();

	void bindViewport(IContextManager& contextManager);

	// actions
	void actionClose();
	void actionNew();
	void actionNone();	// To be removed once all actions are setup.

	std::unique_ptr<IWindow>				mainWindow_;
	std::vector<std::unique_ptr<IView>>		panels_;
	std::vector<std::unique_ptr<IAction>>	actions_;
};

#endif // MAINWINDOW_H
