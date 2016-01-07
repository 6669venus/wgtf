#include "map_status_panel.hpp"
#include "core_variant/variant.hpp"


MapStatusPanel::MapStatusPanel( IComponentContext & context )
    : Depends( context )
{
}


bool MapStatusPanel::addPanel()
{
    auto uiFramework = this->get< IUIFramework >();
    auto uiApplication = this->get< IUIApplication >();

    if ((uiFramework == nullptr) ||
        (uiApplication == nullptr))
    {
        return false;
    }
    mapStatusView_ = uiFramework->createView(
        "MapsSettings/MapStatusPanel.qml",
        IUIFramework::ResourceType::Url );
    uiApplication->addView( *mapStatusView_ );
    return true;
}


void MapStatusPanel::removePanel()
{
    auto uiApplication = this->get< IUIApplication >();
    if (uiApplication == nullptr)
    {
        return;
    }
    uiApplication->removeView( *mapStatusView_ );
}
