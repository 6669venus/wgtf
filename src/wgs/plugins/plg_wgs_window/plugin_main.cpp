#include "dependency_system/i_interface.hpp"
#include "generic_plugin/generic_plugin.hpp"
#include "window_manager.hpp"
#include "qt_common/i_qt_framework.hpp"

#include "ui_framework/i_ui_application.hpp"
#include "ui_framework/i_ui_framework.hpp"
#include "ui_framework/i_window.hpp"


//==============================================================================
class WGSWindowPlugin
	: public PluginMain
{
private:
	WindowManager mainWindow_;

public:
	//==========================================================================
	WGSWindowPlugin(IComponentContext & contextManager)
	{

	}

	//==========================================================================
	bool PostLoad( IComponentContext & contextManager )
	{
		return true;
	}

	//==========================================================================
	void Initialise( IComponentContext & contextManager )
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		auto uiFramework = contextManager.queryInterface< IUIFramework >();

		mainWindow_.init( *uiApplication, *uiFramework );
	}

	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		mainWindow_.fini();
		return true;
	}

	//==========================================================================
	void Unload( IComponentContext & contextManager )
	{
	}

};

PLG_CALLBACK_FUNC(WGSWindowPlugin)