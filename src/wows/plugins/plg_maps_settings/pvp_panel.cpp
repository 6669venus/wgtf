#include "pvp_panel.hpp"
#include "core_variant/variant.hpp"


PvpPanel::PvpPanel( IComponentContext & context )
	: Depends( context )
{
}


bool PvpPanel::addPanel()
{
	auto uiFramework = this->get< IUIFramework >();
	auto uiApplication = this->get< IUIApplication >();

	if ((uiFramework == nullptr) ||
		(uiApplication == nullptr))
	{
		return false;
	}
	pvpPanelView_ = uiFramework->createView(
		"MapsSettings/PvpPanel.qml",
		IUIFramework::ResourceType::Url );
	uiApplication->addView( *pvpPanelView_ );
	return true;
}


void PvpPanel::removePanel()
{
	auto uiApplication = this->get< IUIApplication >();
	if (uiApplication == nullptr)
	{
		return;
	}
	uiApplication->removeView( *pvpPanelView_ );
}
