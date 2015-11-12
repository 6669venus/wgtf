#pragma once
#ifndef _CONTROLS_TEST_PANEL_HPP
#define _CONTROLS_TEST_PANEL_HPP

#include "core_ui_framework/i_view.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_dependency_system/depends.hpp"


#include <memory>


class IUIFramework;
class IUIApplication;


class ControlsTestPanel
	: Depends< IUIFramework, IUIApplication >
{
	typedef Depends< IUIFramework, IUIApplication > DepsBase;

public:
	ControlsTestPanel( IComponentContext & context );

	bool addPanel();
	void removePanel();

private:
	std::unique_ptr< IView > controlsView_;
};


#endif // _CONTROLS_TEST_PANEL_HPP

