#pragma once
#ifndef _PVP_RANKED_PANEL_HPP
#define _PVP_RANKED_PANEL_HPP

#include "core_ui_framework/i_view.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"

#include <memory>

class PvpRankedPanel
    : Depends< IUIFramework, IUIApplication >
{
    typedef Depends< IUIFramework, IUIApplication > DepsBase;

public:
    PvpRankedPanel( IComponentContext & context );

    bool addPanel();
    void removePanel();

private:
    std::unique_ptr< IView > pvpRankedView_;
};

#endif //_PVP_RANKED_PANEL_HPP
