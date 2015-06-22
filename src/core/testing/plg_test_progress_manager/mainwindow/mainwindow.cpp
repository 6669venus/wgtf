#include "mainwindow.hpp"

#include "command_system/command_system_provider.hpp"
#include "commands/test_command.hpp"
#include "generic_plugin/interfaces/i_context_manager.hpp"

#include "ui_framework/i_ui_application.hpp"
#include "ui_framework/i_ui_framework.hpp"
#include "ui_framework/i_action.hpp"

//==============================================================================
MainWindow::MainWindow()
	: contextManager_( nullptr )
{
}

//==============================================================================
MainWindow::~MainWindow()
{
}

//==============================================================================
void MainWindow::init( IUIApplication * uiApplication, IContextManager & contextManager )
{
	contextManager_ = &contextManager;

	createActions();

	uiApplication->addAction( *testCommand1_ );
	uiApplication->addAction( *testCommand2_ );
}

//==============================================================================
void MainWindow::fini()
{
	testCommand2_.reset();
	testCommand1_.reset();
}

// =============================================================================
void MainWindow::createActions()
{
	IUIFramework * uiFramework = contextManager_->queryInterface< IUIFramework >();
	assert( uiFramework );

	// TestCommand1/TestCommand2 QActions
	testCommand1_ = uiFramework->createAction( "TestCommand1", std::bind( &MainWindow::executeTestCommand1, this ) );
	testCommand2_ = uiFramework->createAction( "TestCommand2", std::bind( &MainWindow::executeTestCommand2, this ) );
}

// =============================================================================
void MainWindow::executeTestCommand1()
{
	// Queue the TestCommand1
	CommandSystemProvider * commandSystemProvider = contextManager_->queryInterface< CommandSystemProvider >();
	assert( commandSystemProvider );
	if (commandSystemProvider == nullptr)
	{
		return;
	}

	const char * commandId = getClassIdentifier<TestCommand1>();

	// Tell the Progress Manager to handle this command
	commandSystemProvider->notifyHandleCommandQueued( commandId );
	auto command = commandSystemProvider->executeCommand( commandId );
}

// =============================================================================
void MainWindow::executeTestCommand2()
{
	// Queue the TestCommand2
	CommandSystemProvider * commandSystemProvider = contextManager_->queryInterface< CommandSystemProvider >();
	assert( commandSystemProvider );
	if (commandSystemProvider == nullptr)
	{
		return;
	}

	const char * commandId = getClassIdentifier<TestCommand2>();

	// Tell the Progress Manager to handle this command
	commandSystemProvider->notifyHandleCommandQueued( commandId );
	auto command = commandSystemProvider->executeCommand( commandId );
}

