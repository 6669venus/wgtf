#pragma once
#ifndef _CUSTOM_PANEL_HPP
#define _CUSTOM_PANEL_HPP
 
#include "core_ui_framework/i_view.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_dependency_system/depends.hpp"

#include <memory>

class IUIApplication;

namespace wgt
{
	class IViewCreator;
}

class CustomPanel
	: Depends< IUIApplication, wgt::IViewCreator >
{
public:
	CustomPanel( IComponentContext & context );

	bool addPanel();
	void removePanel();

private:
	std::unique_ptr< IView > customView_;
};
 
 
#endif // _CUSTOM_PANEL_HPP