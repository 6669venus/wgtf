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
class IDataSourceManager;
class ICommandManager;
class IDefinitionManager;
class IReflectionController;

class TestUI
	: Depends<
		IDefinitionManager,
		ICommandManager,
		IReflectionController,
		IDataSourceManager >
{
	typedef Depends<
		IDefinitionManager,
		ICommandManager,
		IReflectionController,
		IDataSourceManager > DepsBase;
public:
	explicit TestUI( IComponentContext & context );
	~TestUI();

	void init( IUIApplication & uiApplication, IUIFramework & uiFramework );
	void fini();

private:
	void createActions( IUIFramework & uiFramework );

	void destroyActions();
	void destroyViews( size_t idx );
	void destroyAllViews();

	void addActions( IUIApplication & uiApplication );
	void addViews( IUIApplication & uiApplication );

	void undo();
	void redo();
	bool canUndo() const;
	bool canRedo() const;

	void open();
	void close();
	bool canOpen() const;
	bool canClose() const;

	void createViews( IUIFramework & uiFramework, IDataSource* dataSrc );
	void removeViews( size_t idx );

	IUIApplication * app_;
	IUIFramework* fw_;

	std::unique_ptr< IAction > testUndo_;
	std::unique_ptr< IAction > testRedo_;

	std::unique_ptr< IAction > testOpen_;
	std::unique_ptr< IAction > testClose_;

	std::vector< IDataSource* > dataSrcs_;

	typedef std::vector< std::unique_ptr< IView > > TestViews;
	TestViews test1Views_;
	TestViews test2Views_;
};


#endif // TEST_UI_H
