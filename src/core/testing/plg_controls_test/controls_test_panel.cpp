#include "controls_test_panel.hpp"
#include "core_variant/variant.hpp"


ControlsTestPanel::ControlsTestPanel( IComponentContext & context )
	: Depends( context )
{
}


bool ControlsTestPanel::addPanel()
{
	auto uiFramework = this->get< IUIFramework >();
	auto uiApplication = this->get< IUIApplication >();
	
	if ((uiFramework == nullptr) ||
		(uiApplication == nullptr))
	{
		return false;
	}

	controlsView_ = uiFramework->createView(
		"plg_controls_test/controls_test_panel.qml",
		IUIFramework::ResourceType::Url );

	uiApplication->addView( *controlsView_ );
	return true;
}


void ControlsTestPanel::removePanel()
{
	auto uiApplication = this->get< IUIApplication >();
	if (uiApplication == nullptr)
	{
		return;
	}

	uiApplication->removeView( *controlsView_ );
}

