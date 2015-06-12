#ifndef I_MENU_HPP
#define I_MENU_HPP

class IAction;

class IMenu
{
public:
	virtual ~IMenu() {}

	virtual const char * path() const = 0;

	virtual void addAction( IAction & action, const char * path ) = 0;
	virtual void removeAction( IAction & action ) = 0;
};

#endif//I_MENU_HPP