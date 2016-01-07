#include "color_picker.hpp"
#include "core_variant/variant.hpp"
#include "color_picker_context.hpp"
#include "core_reflection/property_accessor.hpp"

ColorPicker::ColorPicker( IComponentContext & context )
	: Depends( context )
{
	auto definitionManager = this->get< IDefinitionManager >();
	colorPickerContext_ = definitionManager->create< ColorPickerContext >();
}
 
 
bool ColorPicker::addPanel()
{
    auto uiFramework = this->get< IUIFramework >();
	auto uiApplication = this->get< IUIApplication >();
	
	if ((uiFramework == nullptr) ||
		(uiApplication == nullptr))
	{
		return false;
	}

	
	colorView_ = uiFramework->createView(
		"plg_color_picker/color_picker_panel.qml",
		IUIFramework::ResourceType::Url, colorPickerContext_ );

	uiApplication->addView( *colorView_ );
	return true;
}
 
 
void ColorPicker::removePanel()
{
	auto uiApplication = this->get< IUIApplication >();
	if (uiApplication == nullptr)
	{
		return;
	}

	uiApplication->removeView( *colorView_ );
}


