#ifndef I_UI_APPLICATION_HPP
#define I_UI_APPLICATION_HPP

#include "dependency_system/i_interface.hpp"
#include "generic_plugin_system/interfaces/i_application.hpp"
#include "layout_hint.hpp"

class IAction;
class IComponent;
class IWindow;
class IView;

class IUIApplication : public Implements< IApplication >
{
public:
	virtual ~IUIApplication() {}

	// TODO: windowId needs to be merged into LayoutHint
	// TODO: shortcut should not be explicitly set when adding an action
	virtual void addWindow( IWindow & window ) = 0;
	virtual void addFrame( IView & view, LayoutHint & hint = LayoutHint(), 
		const char * windowId = nullptr ) = 0;
	virtual void addPanel( IView & view, LayoutHint & hint = LayoutHint(), 
		const char * windowId = nullptr ) = 0;
	virtual void addAction( IAction & action, const char * path = nullptr,
		const char * windowId = nullptr, const char * shortcut = nullptr ) = 0;
};

#endif//I_UI_APPLICATION_HPP