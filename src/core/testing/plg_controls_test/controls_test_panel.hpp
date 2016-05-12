#pragma once
#ifndef _CONTROLS_TEST_PANEL_HPP
#define _CONTROLS_TEST_PANEL_HPP

#include "core_ui_framework/i_view.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/object_handle.hpp"
#include <memory>

class IUIApplication;

namespace wgt
{
	class IViewCreator;
}

class ControlsTestPanel
	: Depends< wgt::IViewCreator, IUIApplication, IDefinitionManager >
{
public:
	ControlsTestPanel( IComponentContext & context );

	bool addPanel();
	void removePanel();

private:
	std::unique_ptr< IView > controlsView_;
	ObjectHandle controlData_;
};


#endif // _CONTROLS_TEST_PANEL_HPP

