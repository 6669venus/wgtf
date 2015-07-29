#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include "qt_common/qt_window.hpp"

class IAction;
class IUIApplication;
class IUIFramework;
class IView;
class IWindow;
class IComponentContext;

class MainWindow
{
public:
	explicit MainWindow();
	~MainWindow();

	void init(IComponentContext& contextManager);
	void fini();

private:
	void createActions(IUIFramework& uiFramework, IUIApplication& uiApplication);
	void destroyActions();

	void createViews(IComponentContext& contextManager, IUIFramework& uiFramework, IUIApplication& uiApplication);
	void destroyViews();

	void bindViewport(IComponentContext& contextManager);

	// actions
	void actionClose();
	void actionNew();
	void actionNone();	// To be removed once all actions are setup.

	std::unique_ptr<IWindow>				mainWindow_;
	std::vector<std::unique_ptr<IView>>		panels_;
	std::vector<std::unique_ptr<IAction>>	actions_;
};

#endif // MAINWINDOW_H
