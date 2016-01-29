#include "custom_panel.hpp"
#include "core_variant/variant.hpp"
 
CustomPanel::CustomPanel( IComponentContext & context )
	: Depends( context )
{
}
 
 
bool CustomPanel::addPanel()
{
    auto uiFramework = this->get< IUIFramework >();
	auto uiApplication = this->get< IUIApplication >();
	
	if ((uiFramework == nullptr) ||
		(uiApplication == nullptr))
	{
		return false;
	}

	customView_ = uiFramework->createView(
		"plg_custom_panel/custom_panel.qml",
		IUIFramework::ResourceType::Url );

	uiApplication->addView( *customView_ );
	return true;
}
 
 
void CustomPanel::removePanel()
{
	auto uiApplication = this->get< IUIApplication >();
	if (uiApplication == nullptr)
	{
		return;
	}

	uiApplication->removeView( *customView_ );
}