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
TestUI::TestUI( IComponentContext & context )
	: DepsBase( context )
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
	fw_ = &uiFramework;

	createActions( uiFramework );
	addActions( uiApplication );
}

//------------------------------------------------------------------------------
void TestUI::fini()
{
	destroyAllViews();
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

	// hook open/close
	testOpen_ = uiFramework.createAction(
		"Open", 
		std::bind( &TestUI::open, this ),
		std::bind( &TestUI::canOpen, this ) );

	testClose_ = uiFramework.createAction(
		"Close", 
		std::bind( &TestUI::close, this ),
		std::bind( &TestUI::canClose, this ) );
	
	ICommandManager * commandSystemProvider =
		get< ICommandManager >();
	assert( commandSystemProvider );
	if (commandSystemProvider == NULL)
	{
		return;
	}
}

// =============================================================================
void TestUI::createViews( IUIFramework & uiFramework, IDataSource* dataSrc )
{
	auto defManager = get<IDefinitionManager>(); 
	assert( defManager != nullptr );
	auto controller = get<IReflectionController>();
	assert( controller != nullptr );

	auto model = std::unique_ptr< ITreeModel >(
		new ReflectedTreeModel( dataSrc->getTestPage(), *defManager, controller ) );

	test1Views_.emplace_back( uiFramework.createView( 
		"qrc:///testing_ui_main/test_tree_panel.qml",
		IUIFramework::ResourceType::Url, std::move( model ) ) );

	model = std::unique_ptr< ITreeModel >(
		new ReflectedTreeModel( dataSrc->getTestPage2(), *defManager, controller ) );

	test2Views_.emplace_back( uiFramework.createView( 
		"qrc:///testing_ui_main/test_tree_panel.qml",
		IUIFramework::ResourceType::Url, std::move( model ) ) );
}

// =============================================================================
void TestUI::destroyActions()
{
	assert( app_ != nullptr );
	app_->removeAction( *testRedo_ );
	app_->removeAction( *testUndo_ );
	app_->removeAction( *testOpen_ );
	app_->removeAction( *testClose_ );
	testRedo_.reset();
	testUndo_.reset();
	testOpen_.reset();
	testClose_.reset();
}

// =============================================================================
void TestUI::destroyViews( size_t idx )
{
	assert( test1Views_.size() == test2Views_.size() );
	removeViews( idx );
	test1Views_.erase( test1Views_.begin() + idx );
	test2Views_.erase( test2Views_.begin() + idx );
}

// =============================================================================
void TestUI::destroyAllViews()
{
	assert( test1Views_.size() == test2Views_.size() );
	for (int i = 0; i < test1Views_.size(); ++i)
	{
		removeViews( i );
	}
	test1Views_.resize(0);
	test2Views_.resize(0);
}

// =============================================================================
void TestUI::addActions( IUIApplication & uiApplication )
{
	uiApplication.addAction( *testUndo_ );
	uiApplication.addAction( *testRedo_ );
	uiApplication.addAction( *testOpen_ );
	uiApplication.addAction( *testClose_ );
}

// =============================================================================
void TestUI::addViews( IUIApplication & uiApplication )
{
	uiApplication.addView( *test1Views_.back() );
	uiApplication.addView( *test2Views_.back() );
}

void TestUI::removeViews( size_t i )
{
	assert( app_ != nullptr );
	app_->removeView( *test1Views_[i] );
	app_->removeView( *test2Views_[i] );
}

void TestUI::undo()
{
	ICommandManager * commandSystemProvider =
		get< ICommandManager >();
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
		get< ICommandManager >();
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
		get< ICommandManager >();
	if (commandSystemProvider == NULL)
	{
		return false;
	}
	return commandSystemProvider->canUndo();
}

bool TestUI::canRedo() const
{
	ICommandManager * commandSystemProvider =
		get< ICommandManager >();
	if (commandSystemProvider == NULL)
	{
		return false;
	}
	return commandSystemProvider->canRedo();
}

void TestUI::open()
{
	assert(test1Views_.size() < 5);
	auto dataSrcMngr = get<IDataSourceManager>();
	assert( dataSrcMngr != nullptr );

	IDataSource* ds =  dataSrcMngr->openDataSource();
	dataSrcs_.push_back( ds );
	createViews( *fw_, ds );
	addViews( *app_ );
}

void TestUI::close()
{
	assert( dataSrcs_.size() > 0 );

	IDataSource* ds = dataSrcs_.back();
	dataSrcs_.pop_back();
	destroyViews( dataSrcs_.size() );

	auto dataSrcMngr = get<IDataSourceManager>();
	assert( dataSrcMngr != nullptr );
	dataSrcMngr->closeDataSource( ds );
}

bool TestUI::canOpen() const
{
	assert(test1Views_.size() == test2Views_.size());
	return test1Views_.size() < 5;
}

bool TestUI::canClose() const
{
	assert(test1Views_.size() == test2Views_.size());
	return test1Views_.size() > 0;
}

