#include "command_system/i_command_manager.hpp"
#include "commands/test_command.hpp"
#include "generic_plugin/generic_plugin.hpp"
#include "mainwindow/mainwindow.hpp"


//==============================================================================
class TestProgressManagerPlugin
	: public PluginMain
{
private:
	MainWindow mainWindow_;

	std::unique_ptr< TestCommand1 > testCommand1_;
	std::unique_ptr< TestCommand2 > testCommand2_;

public:
	//==========================================================================
	TestProgressManagerPlugin(IContextManager & contextManager )
		: testCommand1_( new TestCommand1 )
		, testCommand2_( new TestCommand2 )
	{
	}

	//==========================================================================
	bool PostLoad( IContextManager & contextManager )
	{
		Variant::setMetaTypeManager( contextManager.queryInterface< IMetaTypeManager >() );

		return true;
	}

	//==========================================================================
	void Initialise( IContextManager & contextManager )
	{
		ICommandManager* pCommandSystemProvider = contextManager.queryInterface< ICommandManager >();

		if (pCommandSystemProvider == nullptr)
		{
			return;
		}

		// register test commands
		pCommandSystemProvider->registerCommand( testCommand1_.get() );
		pCommandSystemProvider->registerCommand( testCommand2_.get() );

		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		if (uiApplication == nullptr)
		{
			return;
		}

		mainWindow_.init( uiApplication, contextManager );
	}

	//==========================================================================
	bool Finalise( IContextManager & contextManager )
	{
		mainWindow_.fini();

		ICommandManager * commandSystemProvider =
			contextManager.queryInterface< ICommandManager >();

		if (commandSystemProvider)
		{
			commandSystemProvider->deregisterCommand( testCommand1_->getId() );
			commandSystemProvider->deregisterCommand( testCommand2_->getId() );
		}

		return true;
	}

	//==========================================================================
	void Unload( IContextManager & contextManager )
	{
	}

};


PLG_CALLBACK_FUNC( TestProgressManagerPlugin )