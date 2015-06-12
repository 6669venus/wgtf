#ifndef I_WINDOW_HPP
#define I_WINDOW_HPP

#include <vector>
#include <memory>

class IMenu;
class IRegion;

typedef std::vector< std::unique_ptr<IMenu> > Menus;
typedef std::vector< std::unique_ptr<IRegion> > Regions;

class IWindow
{
public:
	virtual ~IWindow() {}

	virtual const char * id() = 0;

	virtual void update() = 0;
	virtual void close() = 0;

	virtual void show() = 0;
	virtual void hide() = 0;

	virtual const Menus & menus() const = 0;
	virtual const Regions & regions() const = 0;
};

#endif I_WINDOW_HPP