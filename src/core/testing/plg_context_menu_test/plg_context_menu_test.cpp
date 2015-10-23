#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_qt_common/i_qt_framework.hpp"
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

		uiFramework->loadActionData( ":/testing/actiondata", IUIFramework::ResourceType::File );
	}
};

PLG_CALLBACK_FUNC( ContextMenuTest )

