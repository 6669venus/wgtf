#include "test_ui.hpp"
#include "command_system/command_system_provider.hpp"
#include "command_system/compound_command.hpp"
#include "reflection/interfaces/i_reflection_property_setter.hpp"
#include "interfaces/i_datasource.hpp"

#include "test_tree_model.hpp"

#include "data_model/reflection/reflected_tree_model.hpp"

#include "ui_framework/i_action.hpp"
#include "ui_framework/i_ui_application.hpp"
#include "ui_framework/i_ui_framework.hpp"
#include "ui_framework/i_view.hpp"
#include "ui_framework/i_window.hpp"

#include "copy_paste_system/i_copy_paste_manager.hpp"

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
	
	CommandSystemProvider * commandSystemProvider =
		Context::queryInterface< CommandSystemProvider >();
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
}

// =============================================================================
void TestUI::createViews( IUIFramework & uiFramework )
{
	auto dataSrc = Context::queryInterface<IDataSource>();
	auto propertySetter = Context::queryInterface<IReflectionPropertySetter>();

	auto model = std::unique_ptr< ITreeModel >(
		new ReflectedTreeModel( dataSrc->getTestPage(), propertySetter ) );
	testView_ = uiFramework.createView( 
		"qrc:///testing/test_tree_panel.qml",
		IUIFramework::ResourceType::Url, model );

	model = std::unique_ptr< ITreeModel >(
		new ReflectedTreeModel( dataSrc->getTestPage2(), propertySetter ) );
	test2View_ = uiFramework.createView( 
		"qrc:///testing/test_tree_panel.qml",
		IUIFramework::ResourceType::Url, model );

	model = std::unique_ptr< ITreeModel >( new TestTreeModel() );
	randomDataView_ = uiFramework.createView( 
		"qrc:///testing/test_tree_panel.qml",
		IUIFramework::ResourceType::Url, model );
}

// =============================================================================
void TestUI::destroyActions()
{
	testCreateMacro_.reset();
	testBatchCommand_.reset();
	testRedo_.reset();
	testUndo_.reset();
}

// =============================================================================
void TestUI::destroyViews()
{
	randomDataView_.reset();
	test2View_.reset();
	testView_.reset();
}

// =============================================================================
void TestUI::addActions( IUIApplication & uiApplication )
{
	uiApplication.addAction( *testUndo_ );
	uiApplication.addAction( *testRedo_ );
	uiApplication.addAction( *testBatchCommand_ );
	uiApplication.addAction( *testCreateMacro_ );
}

// =============================================================================
void TestUI::addViews( IUIApplication & uiApplication )
{
	uiApplication.addView( *testView_ );
	uiApplication.addView( *test2View_ );
	uiApplication.addView( *randomDataView_ );
}

void TestUI::batchAction( )
{
	CommandSystemProvider * commandSystemProvider =
		Context::queryInterface< CommandSystemProvider >();
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
	auto propertyAccessor = obj.getDefinition()->bindProperty( "TextField", obj );
	auto propertyAccessor2 = obj.getDefinition()->bindProperty( "Number", obj );
	commandSystemProvider->beginBatchCommand();
	propertySetter->setDataValue( propertyAccessor, "Wargaming.net" );
	propertySetter->setDataValue( propertyAccessor2, 3333 );
	commandSystemProvider->endBatchCommand();
}

void TestUI::createMacro()
{
	CommandSystemProvider * commandSystemProvider =
		Context::queryInterface< CommandSystemProvider >();
	assert( commandSystemProvider );
	if (commandSystemProvider == nullptr)
	{
		return;
	}
	auto & history = commandSystemProvider->getHistory();
	commandSystemProvider->createCompoundCommand( const_cast<GenericList &>(history) );
}

void TestUI::undo()
{
	CommandSystemProvider * commandSystemProvider =
		Context::queryInterface< CommandSystemProvider >();
	assert( commandSystemProvider );
	if (commandSystemProvider == NULL)
	{
		return;
	}
	commandSystemProvider->undo();
}

void TestUI::redo()
{
	CommandSystemProvider * commandSystemProvider =
		Context::queryInterface< CommandSystemProvider >();
	assert( commandSystemProvider );
	if (commandSystemProvider == NULL)
	{
		return;
	}
	commandSystemProvider->redo();
}

bool TestUI::canUndo() const
{
	CommandSystemProvider * commandSystemProvider =
		Context::queryInterface< CommandSystemProvider >();
	if (commandSystemProvider == NULL)
	{
		return false;
	}
	return commandSystemProvider->canUndo();
}

bool TestUI::canRedo() const
{
	CommandSystemProvider * commandSystemProvider =
		Context::queryInterface< CommandSystemProvider >();
	if (commandSystemProvider == NULL)
	{
		return false;
	}
	return commandSystemProvider->canRedo();
}
