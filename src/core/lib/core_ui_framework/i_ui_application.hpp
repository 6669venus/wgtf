#ifndef I_UI_APPLICATION_HPP
#define I_UI_APPLICATION_HPP

#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "layout_hint.hpp"

#include <map>
#include <memory>

class IAction;
class IComponent;
class IMenu;
class IWindow;
class IView;

typedef std::map< std::string, IWindow * > Windows;

class IUIApplication : public Implements< IApplication >
{
public:

	typedef std::function<void(void)> VoidCallback;

public:
	virtual ~IUIApplication() {}

	virtual void addWindow( IWindow & window ) = 0;
	virtual void removeWindow( IWindow & window ) = 0;
	virtual void addView( IView & view ) = 0;
	virtual void removeView( IView & view ) = 0;
	virtual void addMenu( IMenu & menu ) = 0;
	virtual void removeMenu( IMenu & menu ) = 0;
	virtual void addAction( IAction & action ) = 0;
	virtual void removeAction( IAction & action ) = 0;
	virtual const Windows & windows() const = 0;

	//! \brief use to register a callback for application update.  this function uses
	//	   internal application update callback to forward an update as often as it needs.
	virtual void connectOnUpdate(VoidCallback callback) = 0;
};

#endif//I_UI_APPLICATION_HPP