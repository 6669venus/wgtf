#include "color_picker.hpp"
#include "core_variant/variant.hpp"
#include "metadata/color_picker.mpp"
 
ColorPicker::ColorPicker( IComponentContext & context )
	: Depends( context )
{
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
		IUIFramework::ResourceType::Url );

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



QColor ColorPicker::grabScreenColor(const QPoint &p)
{
    const QDesktopWidget *desktop = QApplication::desktop();
    const QPixmap pixmap = QGuiApplication::screens().at(desktop->screenNumber())->grabWindow(desktop->winId(),
                                                                                              p.x(), p.y(), 1, 1);
    QImage i = pixmap.toImage();
    return i.pixel(0, 0);
}
