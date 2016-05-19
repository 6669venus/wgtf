#include "timeline_panel.hpp"
#include "core_variant/variant.hpp"
 
 
namespace wgt
{
TimelinePanel::TimelinePanel( IComponentContext & context )
    : Depends( context )
{
}
 
 
bool TimelinePanel::addPanel()
{
    auto uiFramework = this->get< IUIFramework >();
    auto uiApplication = this->get< IUIApplication >();
     
    if ((uiFramework == nullptr) ||
        (uiApplication == nullptr))
    {
        return false;
    }
    timelineView_ = uiFramework->createView(
        "PlgTimelinePanel/TimelinePanel.qml",
        IUIFramework::ResourceType::Url );
    uiApplication->addView( *timelineView_ );
    return true;
}
 
 
void TimelinePanel::removePanel()
{
    auto uiApplication = this->get< IUIApplication >();
    if (uiApplication == nullptr)
    {
        return;
    }
    uiApplication->removeView( *timelineView_ );
}
} // end namespace wgt
