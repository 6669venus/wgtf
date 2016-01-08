#include "pve_panel.hpp"
#include "core_variant/variant.hpp"


PvePanel::PvePanel( IComponentContext & context )
    : Depends( context )
{
}


bool PvePanel::addPanel()
{
    auto uiFramework = this->get< IUIFramework >();
    auto uiApplication = this->get< IUIApplication >();

    if ((uiFramework == nullptr) ||
        (uiApplication == nullptr))
    {
        return false;
    }
    pvePanelView_ = uiFramework->createView(
        "MapsSettings/PvePanel.qml",
        IUIFramework::ResourceType::Url );
    uiApplication->addView( *pvePanelView_ );
    return true;
}


void PvePanel::removePanel()
{
    auto uiApplication = this->get< IUIApplication >();
    if (uiApplication == nullptr)
    {
        return;
    }
    uiApplication->removeView( *pvePanelView_ );
}
