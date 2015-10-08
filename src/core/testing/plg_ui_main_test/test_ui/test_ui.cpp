#include "test_ui.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_command_system/compound_command.hpp"
#include "core_reflection/interfaces/i_reflection_property_setter.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "interfaces/i_datasource.hpp"

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

	addActions( uiApplication );
	addViews( uiApplication );
}

//------------------------------------------------------------------------------
void TestUI::fini()
{
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
		"qrc:///testing_ui_main/test_tree_panel.qml",
		IUIFramework::ResourceType::Url, std::move( model ) );

	model = std::unique_ptr< ITreeModel >(
		new ReflectedTreeModel( dataSrc->getTestPage2(), *defManager, controller ) );
	test2View_ = uiFramework.createView( 
		"qrc:///testing_ui_main/test_tree_panel.qml",
		IUIFramework::ResourceType::Url, std::move( model ) );
}

// =============================================================================
void TestUI::destroyActions()
{
	assert( app_ != nullptr );
	app_->removeAction( *testRedo_ );
	app_->removeAction( *testUndo_ );
	testRedo_.reset();
	testUndo_.reset();
}

// =============================================================================
void TestUI::destroyViews()
{
	removeViews();
	test2View_.reset();
	testView_.reset();
}

// =============================================================================
void TestUI::addActions( IUIApplication & uiApplication )
{
	uiApplication.addAction( *testUndo_ );
	uiApplication.addAction( *testRedo_ );
}

// =============================================================================
void TestUI::addViews( IUIApplication & uiApplication )
{
	uiApplication.addView( *testView_ );
	uiApplication.addView( *test2View_ );
}

void TestUI::removeViews()
{
	assert( app_ != nullptr );
	app_->removeView( *testView_ );
	app_->removeView( *test2View_ );
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

