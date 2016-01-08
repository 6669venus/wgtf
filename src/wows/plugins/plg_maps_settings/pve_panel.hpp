#pragma once
#ifndef _PVE_PANEL_HPP
#define _PVE_PANEL_HPP

#include "core_ui_framework/i_view.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"

#include <memory>

class PvePanel
    : Depends< IUIFramework, IUIApplication >
{
    typedef Depends< IUIFramework, IUIApplication > DepsBase;

public:
    PvePanel( IComponentContext & context );

    bool addPanel();
    void removePanel();

private:
    std::unique_ptr< IView > pvePanelView_;
};

#endif // _PVE_PANEL_HPP
