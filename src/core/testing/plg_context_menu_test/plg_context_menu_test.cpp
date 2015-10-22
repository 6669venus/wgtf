#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_window.hpp"


//==============================================================================
class ContextMenuTest
	: public PluginMain
{
public:
	//==========================================================================
	ContextMenuTest( IComponentContext & contextManager ){}

	//==========================================================================
	bool PostLoad( IComponentContext & contextManager )
	{
		return true;
	}

	//==========================================================================
	void Initialise( IComponentContext & contextManager )
	{
		auto uiFramework = contextManager.queryInterface< IUIFramework >();
		assert( uiFramework != nullptr );

		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		assert( uiApplication != nullptr );

		// Load the action data
		uiFramework->loadActionData( ":/testing_context_menu/actiondata", IUIFramework::ResourceType::File );

		// Create actions and add them to the UI Application
		cmTestOpen_ = uiFramework->createAction( "CMTestExplorerOpen",
			std::bind( &ContextMenuTest::executeOpen, this ),
			std::bind( &ContextMenuTest::canExecuteOpen, this ) );
		uiApplication->addAction( *cmTestOpen_ );

		cmTestCheckOut_ = uiFramework->createAction( "CMTestPerforceCheckOut",
			std::bind( &ContextMenuTest::executeCheckOut, this ) );
		uiApplication->addAction( *cmTestCheckOut_ );

		// Create the view and present it
		testView_ = uiFramework->createView(
			"qrc:///testing_context_menu/test_contextmenu_panel.qml",
			IUIFramework::ResourceType::Url );

		uiApplication->addView( *testView_ );
	}

	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		assert( uiApplication != nullptr );
		
		uiApplication->removeAction( *cmTestOpen_ );
		uiApplication->removeAction( *cmTestCheckOut_ );
		
		cmTestOpen_.reset();
		cmTestCheckOut_.reset();

		uiApplication->removeView( *testView_ );
		testView_ = nullptr;

		return true;
	}
	
	//==========================================================================
	bool canExecuteOpen()
	{
		return true;
	}

	void executeOpen()
	{
		NGT_DEBUG_MSG( "Open file context menu item clicked!\n" );
	}
		
	//==========================================================================
	void executeCheckOut()
	{
		NGT_DEBUG_MSG( "Perforce check out context menu item clicked!\n" );
	}

private:
	
	std::unique_ptr<IView> testView_;
	std::unique_ptr< IAction > cmTestCheckOut_;
	std::unique_ptr< IAction > cmTestOpen_;
};

PLG_CALLBACK_FUNC( ContextMenuTest )

