#pragma once
#ifndef _TIMELINE_PANEL_HPP
#define _TIMELINE_PANEL_HPP
 
#include "core_ui_framework/i_view.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"
 
#include <memory>
 
namespace wgt
{
class TimelinePanel
    : Depends< IUIFramework, IUIApplication >
{
    typedef Depends< IUIFramework, IUIApplication > DepsBase;
 
public:
    TimelinePanel( IComponentContext & context );
 
    bool addPanel();
    void removePanel();
 
private:
    std::unique_ptr< IView > timelineView_;
};
 
}
#endif // _TIMELINE_PANEL_HPP
