#pragma once
#ifndef _CUSTOM_PANEL_HPP
#define _CUSTOM_PANEL_HPP
 
#include "core_ui_framework/i_view.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"

#include <memory>


namespace wgt
{
class CustomPanel
	: Depends< IUIFramework, IUIApplication >
{
	typedef Depends< IUIFramework, IUIApplication > DepsBase;

public:
	CustomPanel( IComponentContext & context );

	bool addPanel();
	void removePanel();

private:
	std::unique_ptr< IView > customView_;
};
 
 
} // end namespace wgt
#endif // _CUSTOM_PANEL_HPP
