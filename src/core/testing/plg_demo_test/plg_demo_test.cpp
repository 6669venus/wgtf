#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_qt_common/i_qt_framework.hpp"

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_view.hpp"


//==============================================================================
class DemoTestPlugin
	: public PluginMain
{
private:
	std::unique_ptr< IView > centralView_;
public:
	//==========================================================================
	DemoTestPlugin(IComponentContext & contextManager )
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
		if ( uiApplication == nullptr )
		{
			return ;
		}
		
		CREATE_QML_VIEW( centralView_, "plg_demo_test/viewport_test_window.qml", nullptr );

		uiApplication->addView( *centralView_ );
	}

	//==========================================================================
	bool Finalise( IComponentContext & contextManager )
	{
		auto uiApplication = contextManager.queryInterface< IUIApplication >();
		if (uiApplication == nullptr)
		{
			return false;
		}
		uiApplication->removeView( *centralView_ );
		centralView_ = nullptr;
		return true;
	}

	//==========================================================================
	void Unload( IComponentContext & contextManager )
	{
	}

};


PLG_CALLBACK_FUNC(DemoTestPlugin)

