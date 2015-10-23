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

	void destroyActions();
	void destroyViews();

	void addActions( IUIApplication & uiApplication );
	void addViews( IUIApplication & uiApplication );

private:
	IUIApplication * app_;
	std::unique_ptr< IAction > testUndo_;
	std::unique_ptr< IAction > testRedo_;
	std::unique_ptr< IView > testView_;
	std::unique_ptr< IView > test2View_;

	void undo( IAction * action );
	void redo( IAction * action );
	bool canUndo( IAction* action ) const;
	bool canRedo( IAction* action ) const;
	void removeViews();

};


#endif // TEST_UI_H
