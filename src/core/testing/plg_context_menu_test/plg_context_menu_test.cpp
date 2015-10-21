#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
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
		assert( uiFramework );

		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		assert( uiApplication );

		// Load the action data
		uiFramework->loadActionData( ":/testing/actiondata", IUIFramework::ResourceType::File );

		// Create an action and add it to the UI Application
		contextMenuTest1_ = uiFramework->createAction( "ContextMenuTest1", 
														std::bind( &ContextMenuTest::executeTest1, this ) );
		uiApplication->addAction( *contextMenuTest1_ );
	}
		
	//==========================================================================
	void executeTest1()
	{
	}

private:
	
	std::unique_ptr< IAction > contextMenuTest1_;
};

PLG_CALLBACK_FUNC( ContextMenuTest )

