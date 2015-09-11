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

	testBatchCommand_ = uiFramework.createAction(
		"BatchCommand", 
		std::bind( &TestUI::batchAction, this ) );

	testCreateMacro_ = uiFramework.createAction(
		"CreateMacro", 
		std::bind( &TestUI::createMacro, this ) );

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
		"testing/test_tree_panel.qml",
		IUIFramework::ResourceType::Url, std::move( model ) );

	model = std::unique_ptr< ITreeModel >(
		new ReflectedTreeModel( dataSrc->getTestPage2(), *defManager, controller ) );
	test2View_ = uiFramework.createView( 
		"testing/test_tree_panel.qml",
		IUIFramework::ResourceType::Url, std::move( model ) );

	auto treeListModel = defManager->create<TreeListModel>();
	treeListModel->init( *defManager, *controller );
	treeListView_ = uiFramework.createView( 
		"testing/test_tree_list_panel.qml",
		IUIFramework::ResourceType::Url, treeListModel );
		
	model = std::unique_ptr< ITreeModel >( new TestTreeModel() );
	randomDataView_ = uiFramework.createView( 
		"testing/test_tree_panel.qml",
		IUIFramework::ResourceType::Url, std::move( model ) );
	
	std::unique_ptr< IListModel > listModel( new TestListModel() );
	randomListView_ = uiFramework.createView(
		"testing/test_list_panel.qml",
		IUIFramework::ResourceType::Url, std::move( listModel ) );
}

// =============================================================================
void TestUI::createWindows( IUIFramework & uiFramework )
{
	modalDialog_ = uiFramework.createWindow( 
		"testing/test_custom_dialog.qml",
		IUIFramework::ResourceType::Url );
	if (modalDialog_ != nullptr)
	{
		modalDialog_->hide();
	}
}

// =============================================================================
void TestUI::destroyActions()
{
	testModalDialog_.reset();
	testCreateMacro_.reset();
	testBatchCommand_.reset();
	testRedo_.reset();
	testUndo_.reset();
}

// =============================================================================
void TestUI::destroyViews()
{
	randomListView_.reset();
	randomDataView_.reset();
	treeListView_.reset();
	test2View_.reset();
	testView_.reset();
}

// =============================================================================
void TestUI::destroyWindows()
{
	modalDialog_.reset();
}

// =============================================================================
void TestUI::addActions( IUIApplication & uiApplication )
{
	uiApplication.addAction( *testUndo_ );
	uiApplication.addAction( *testRedo_ );
	uiApplication.addAction( *testBatchCommand_ );
	uiApplication.addAction( *testCreateMacro_ );
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
}

// =============================================================================
void TestUI::addWindows( IUIApplication & uiApplication )
{
	uiApplication.addWindow( *modalDialog_ );
}

void TestUI::batchAction( )
{
	auto defManager = Context::queryInterface<IDefinitionManager>();
	assert( defManager != nullptr );
	if (defManager == nullptr)
	{
		return;
	}
	ICommandManager * commandSystemProvider =
		Context::queryInterface< ICommandManager >();
	assert( commandSystemProvider );
	if (commandSystemProvider == nullptr)
	{
		return;
	}
	auto propertySetter = Context::queryInterface<IReflectionPropertySetter>();
	if (propertySetter == nullptr)
	{
		return;
	}
	auto dataSrc = Context::queryInterface<IDataSource>();
	const ObjectHandle & obj = dataSrc->getTestPage();
	auto propertyAccessor = obj.getDefinition( *defManager )->bindProperty( "TextField", obj );
	auto propertyAccessor2 = obj.getDefinition( *defManager )->bindProperty( "Number", obj );
	commandSystemProvider->beginBatchCommand();
	propertySetter->setDataValue( propertyAccessor, "Wargaming.net" );
	propertySetter->setDataValue( propertyAccessor2, 3333 );
	commandSystemProvider->endBatchCommand();
}

void TestUI::createMacro()
{
	ICommandManager * commandSystemProvider =
		Context::queryInterface< ICommandManager >();
	assert( commandSystemProvider );
	if (commandSystemProvider == nullptr)
	{
		return;
	}
	auto & history = commandSystemProvider->getHistory();
	commandSystemProvider->createMacro( const_cast<VariantList &>(history) );
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

