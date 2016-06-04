#include "custom_panel.hpp"
#include "core_logging/logging.hpp"
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

	if (customView_ != nullptr)
	{
		uiApplication->addView( *customView_ );
	}
	else
	{
		NGT_ERROR_MSG( "Failed to load qml\n" );
	}
	return true;
}
 
 
void CustomPanel::removePanel()
{
	auto uiApplication = this->get< IUIApplication >();
	if (uiApplication == nullptr)
	{
		return;
	}

	if (customView_ != nullptr)
	{
		uiApplication->removeView( *customView_ );
		customView_ = nullptr;
	}
}