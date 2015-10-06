#include "test_ui.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_command_system/compound_command.hpp"
#include "core_reflection/interfaces/i_reflection_property_setter.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "interfaces/i_datasource.hpp"

#include "test_tree_model.hpp"
#include "test_list_model.hpp"
#include "tree_list_model.hpp"

#include "core_data_model/reflection/reflected_tree_model.hpp"

#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_window.hpp"

#include "core_copy_paste/i_copy_paste_manager.hpp"

//==============================================================================
TestUI::TestUI()
{
}

//==============================================================================
TestUI::~TestUI()
{

}

//==============================================================================
void TestUI::init( IUIApplication & uiApplication, IUIFramework & uiFramework )
{
	app_ = &uiApplication;
	createActions( uiFramework );
	createViews( uiFramework );
	createWindows( uiFramework );

	addActions( uiApplication );
	addViews( uiApplication );
	addWindows( uiApplication );
}

//------------------------------------------------------------------------------
void TestUI::fini()
{
	destroyWindows();
	destroyViews();
	destroyActions();
}

// =============================================================================
void TestUI::createActions( IUIFramework & uiFramework )
{
	// hook undo/redo
	testUndo_ = uiFramework.createAction(
		"Undo", 
		std::bind( &TestUI::undo, this ),
		std::bind( &TestUI::canUndo, this ) );

	testRedo_ = uiFramework.createAction(
		"Redo", 
		std::bind( &TestUI::redo, this ),
		std::bind( &TestUI::canRedo, this ) );
	
	ICommandManager * commandSystemProvider =
		Context::queryInterface< ICommandManager >();
	assert( commandSystemProvider );
	if (commandSystemProvider == NULL)
	{
		return;
	}

	removeTestPanel_ = uiFramework.createAction(
		"RemoveTestResource", 
		std::bind( &TestUI::removeViews, this ) );

	restoreTestPanel_ = uiFramework.createAction(
		"RestoreTestResource", 
		std::bind( &TestUI::restoreViews, this ) );

	testModalDialog_ = uiFramework.createAction(
		"ShowModalDialog", 
		std::bind( &TestUI::showModalDialog, this ) );
}

// =============================================================================
void TestUI::createViews( IUIFramework & uiFramework )
{
	auto dataSrc = Context::queryInterface<IDataSource>();
	assert( dataSrc != nullptr );
	auto defManager = Context::queryInterface<IDefinitionManager>();
	assert( defManager != nullptr );
	auto controller = Context::queryInterface<IReflectionController>();
	assert( controller != nullptr );
	auto model = std::unique_ptr< ITreeModel >(
		new ReflectedTreeModel( dataSrc->getTestPage(), *defManager, controller ) );
	testView_ = uiFramework.createView( 
		"qrc:///testing/test_tree_panel.qml",
		IUIFramework::ResourceType::Url, std::move( model ) );

	model = std::unique_ptr< ITreeModel >(
		new ReflectedTreeModel( dataSrc->getTestPage2(), *defManager, controller ) );
	test2View_ = uiFramework.createView( 
		"qrc:///testing/test_tree_panel.qml",
		IUIFramework::ResourceType::Url, std::move( model ) );

	auto treeListModel = defManager->create<TreeListModel>();
	treeListModel->init( *defManager, *controller );
	treeListView_ = uiFramework.createView( 
		"qrc:///testing/test_tree_list_panel.qml",
		IUIFramework::ResourceType::Url, treeListModel );
		
	model = std::unique_ptr< ITreeModel >( new TestTreeModel() );
	randomDataView_ = uiFramework.createView( 
		"qrc:///testing/test_tree_panel.qml",
		IUIFramework::ResourceType::Url, std::move( model ) );
	
	std::unique_ptr< IListModel > listModel( new TestListModel() );
	randomListView_ = uiFramework.createView(
		"qrc:///testing/test_list_panel.qml",
		IUIFramework::ResourceType::Url, std::move( listModel ) );

	std::unique_ptr< IListModel > shortListModel( new TestListModel( true ) );
	randomShortListView_ = uiFramework.createView(
		"qrc:///testing/test_list_panel.qml",
		IUIFramework::ResourceType::Url, std::move( shortListModel ) );
}

// =============================================================================
void TestUI::createWindows( IUIFramework & uiFramework )
{
	modalDialog_ = uiFramework.createWindow( 
		"qrc:///testing/test_custom_dialog.qml", 
		IUIFramework::ResourceType::Url );
	if (modalDialog_ != nullptr)
	{
		modalDialog_->hide();
	}
}

// =============================================================================
void TestUI::destroyActions()
{
	assert( app_ != nullptr );
	app_->removeAction( *testModalDialog_ );
	app_->removeAction( *restoreTestPanel_ );
	app_->removeAction( *removeTestPanel_ );
	app_->removeAction( *testRedo_ );
	app_->removeAction( *testUndo_ );
	testModalDialog_.reset();
	restoreTestPanel_.reset();
	removeTestPanel_.reset();
	testRedo_.reset();
	testUndo_.reset();
}

// =============================================================================
void TestUI::destroyViews()
{
	removeViews();
	randomShortListView_.reset();
	randomListView_.reset();
	randomDataView_.reset();
	treeListView_.reset();
	test2View_.reset();
	testView_.reset();
}

// =============================================================================
void TestUI::destroyWindows()
{
	assert( app_ != nullptr );
	app_->removeWindow( *modalDialog_ );
	modalDialog_.reset();
}

// =============================================================================
void TestUI::addActions( IUIApplication & uiApplication )
{
	uiApplication.addAction( *testUndo_ );
	uiApplication.addAction( *testRedo_ );
	uiApplication.addAction( *removeTestPanel_ );
	uiApplication.addAction( *restoreTestPanel_ );
	uiApplication.addAction( *testModalDialog_ );
}

// =============================================================================
void TestUI::addViews( IUIApplication & uiApplication )
{
	uiApplication.addView( *testView_ );
	uiApplication.addView( *test2View_ );
	uiApplication.addView( *treeListView_ );
	uiApplication.addView( *randomDataView_ );
	uiApplication.addView( *randomListView_ );
	uiApplication.addView( *randomShortListView_ );
}

// =============================================================================
void TestUI::addWindows( IUIApplication & uiApplication )
{
	uiApplication.addWindow( *modalDialog_ );
}

void TestUI::removeViews()
{
	assert( app_ != nullptr );
	app_->removeView( *testView_ );
	app_->removeView( *test2View_ );
	app_->removeView( *treeListView_ );
	app_->removeView( *randomDataView_ );
	app_->removeView( *randomListView_ );
	app_->removeView( *randomShortListView_ );
	
}

void TestUI::restoreViews()
{
	assert( app_ != nullptr );
	addViews( *app_ );
}

void TestUI::undo()
{
	ICommandManager * commandSystemProvider =
		Context::queryInterface< ICommandManager >();
	assert( commandSystemProvider );
	if (commandSystemProvider == NULL)
	{
		return;
	}
	commandSystemProvider->undo();
}

void TestUI::redo()
{
	ICommandManager * commandSystemProvider =
		Context::queryInterface< ICommandManager >();
	assert( commandSystemProvider );
	if (commandSystemProvider == NULL)
	{
		return;
	}
	commandSystemProvider->redo();
}

bool TestUI::canUndo() const
{
	ICommandManager * commandSystemProvider =
		Context::queryInterface< ICommandManager >();
	if (commandSystemProvider == NULL)
	{
		return false;
	}
	return commandSystemProvider->canUndo();
}

bool TestUI::canRedo() const
{
	ICommandManager * commandSystemProvider =
		Context::queryInterface< ICommandManager >();
	if (commandSystemProvider == NULL)
	{
		return false;
	}
	return commandSystemProvider->canRedo();
}

void TestUI::showModalDialog()
{
	if (modalDialog_ == nullptr)
	{
		return;
	}
	modalDialog_->showModal();
}

