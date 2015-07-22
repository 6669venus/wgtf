#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <memory>

class IAction;
class IUIApplication;
class IUIFramework;
class IView;
class IWindow;

class WindowManager
{    
public:
    explicit WindowManager();
    ~WindowManager();

	void init(IUIApplication &uiApplication, IUIFramework &uiFramework);
	void fini();

private:
	void createActions(IUIFramework &uiFramework);
	void destroyActions();

	void close();

	void addMenuBar(IUIApplication &uiApplication);

private:
	std::unique_ptr< IWindow > mainWindow_;
	std::unique_ptr< IAction > exitAction_;
};


#endif // WINDOWMANAGER_H
