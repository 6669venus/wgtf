#ifndef TEST_UI_H
#define TEST_UI_H

#include <memory>
#include "core_dependency_system/depends.hpp"

class IAction;
class IUIApplication;
class IUIFramework;
class IView;
class IWindow;
class IDataSource;
class ICommandManager;
class IDefinitionManager;
class IReflectionController;

class TestUI
	: Depends<
		IDefinitionManager,
		ICommandManager,
		IReflectionController,
		IDataSource >
{
	typedef Depends<
		IDefinitionManager,
		ICommandManager,
		IReflectionController,
		IDataSource > DepsBase;
public:
    explicit TestUI( IComponentContext & context );
    ~TestUI();

	void init( IUIApplication & uiApplication, IUIFramework & uiFramework );
	void fini();

private:

	void createActions( IUIFramework & uiFramework );
	void createViews( IUIFramework & uiFramework );
	void createWindows( IUIFramework & uiFramework );

	void destroyActions();
	void destroyViews();
	void destroyWindows();

	void addActions( IUIApplication & uiApplication );
	void addViews( IUIApplication & uiApplication );
	void addWindows( IUIApplication & uiApplication );

private:
	IUIApplication * app_;
	std::unique_ptr< IAction > testUndo_;
	std::unique_ptr< IAction > testRedo_;
	std::unique_ptr< IAction > removeTestPanel_;
	std::unique_ptr< IAction > restoreTestPanel_;
	std::unique_ptr< IAction > testModalDialog_;
	std::unique_ptr< IView > testView_;
	std::unique_ptr< IView > test2View_;
	std::unique_ptr< IView > treeListView_;
	std::unique_ptr< IView > randomDataView_;
	std::unique_ptr< IView > randomListView_;
	std::unique_ptr< IView > randomShortListView_;
	std::unique_ptr< IWindow > modalDialog_;

	void undo();
	void redo();
	bool canUndo() const;
	bool canRedo() const;
	void showModalDialog();
	void removeViews();
	void restoreViews();

};


#endif // TEST_UI_H
