#include "pvp_ranked_panel.hpp"
#include "core_variant/variant.hpp"


PvpRankedPanel::PvpRankedPanel( IComponentContext & context )
    : Depends( context )
{
}


bool PvpRankedPanel::addPanel()
{
    auto uiFramework = this->get< IUIFramework >();
    auto uiApplication = this->get< IUIApplication >();

    if ((uiFramework == nullptr) ||
        (uiApplication == nullptr))
    {
        return false;
    }
    pvpRankedView_ = uiFramework->createView(
        "MapsSettings/PvpRankedPanel.qml",
        IUIFramework::ResourceType::Url );
    uiApplication->addView( *pvpRankedView_ );
    return true;
}


void PvpRankedPanel::removePanel()
{
    auto uiApplication = this->get< IUIApplication >();
    if (uiApplication == nullptr)
    {
        return;
    }
    uiApplication->removeView( *pvpRankedView_ );
}
