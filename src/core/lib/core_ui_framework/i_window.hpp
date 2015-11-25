#ifndef I_WINDOW_HPP
#define I_WINDOW_HPP

#include "wg_types/event.hpp"

#include <vector>
#include <memory>

class IMenu;
class IRegion;
class IUIApplication;

typedef std::vector< std::unique_ptr<IMenu> > Menus;
typedef std::vector< std::unique_ptr<IRegion> > Regions;

class IWindow
{
public:
	virtual ~IWindow() {}

	virtual const char * id() const = 0;
	virtual const char * title() const = 0;

	virtual void update() = 0;
	virtual void close() = 0;

	virtual void show( bool wait = false ) = 0;
	virtual void showMaximized( bool wait = false ) = 0;
	virtual void showModal() {}
	virtual void hide() = 0;

	virtual const Menus & menus() const = 0;
	virtual const Regions & regions() const = 0;

	virtual void setApplication( IUIApplication * application ) = 0;
	virtual IUIApplication * getApplication() const = 0;

	PUBLIC_EVENT( IWindow, CloseEvent )
};

#endif //I_WINDOW_HPP