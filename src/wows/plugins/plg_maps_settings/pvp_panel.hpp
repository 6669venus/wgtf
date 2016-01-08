#pragma once
#ifndef _PVP_PANEL_HPP
#define _PVP_PANEL_HPP

#include "core_ui_framework/i_view.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"

#include <memory>

class PvpPanel
    : Depends< IUIFramework, IUIApplication >
{
    typedef Depends< IUIFramework, IUIApplication > DepsBase;

public:
    PvpPanel( IComponentContext & context );

    bool addPanel();
    void removePanel();

private:
    std::unique_ptr< IView > pvpPanelView_;
};

#endif // _PVP_PANEL_HPP
