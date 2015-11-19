#ifndef I_MENU_HPP
#define I_MENU_HPP

class IAction;

class IMenu
{
public:
	virtual ~IMenu() {}

	virtual const char * path() const = 0;
	virtual const char * windowId() const = 0;

	virtual void update() = 0;

	virtual void addAction( IAction & action ) = 0;
	virtual void removeAction( IAction & action ) = 0;
};

#endif//I_MENU_HPP