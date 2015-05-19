#ifndef TEST_UI_H
#define TEST_UI_H

#include <memory>

class IAction;
class IUIApplication;
class IUIFramework;
class IView;
class IWindow;

class TestUI
{    
public:
    explicit TestUI();
    ~TestUI();

	void init( IUIApplication & uiApplication, IUIFramework & uiFramework );
	void fini();

private:
	void onIdle();

	void createActions();
	void createViews( IUIFramework & uiFramework );

	void destroyActions();
	void destroyViews();

	void addToolBar( IUIApplication & uiApplication );
	void addFrames( IUIApplication & uiApplication );

private:
	std::unique_ptr< IAction > testUndo_;
	std::unique_ptr< IAction > testRedo_;
	std::unique_ptr< IAction > testBatchCommand_;
	std::unique_ptr< IAction > testCreateMacro_;
	std::unique_ptr< IView > testView_;
	std::unique_ptr< IView > test2View_;
	std::unique_ptr< IView > randomDataView_;

	void close();
	void undo();
	void redo();
	bool canUndo();
	bool canRedo();

	void batchAction();
	void createMacro();

};


#endif // TEST_UI_H
