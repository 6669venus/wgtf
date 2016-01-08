#pragma once
#ifndef _MAP_STATUS_PANEL_HPP
#define _MAP_STATUS_PANEL_HPP

#include "core_ui_framework/i_view.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"

#include <memory>

class MapStatusPanel
	: Depends< IUIFramework, IUIApplication >
{
	typedef Depends< IUIFramework, IUIApplication > DepsBase;

public:
	MapStatusPanel( IComponentContext & context );

	bool addPanel();
	void removePanel();

private:
	std::unique_ptr< IView > mapStatusView_;
};

#endif // _MAP_STATUS_PANEL_HPP
