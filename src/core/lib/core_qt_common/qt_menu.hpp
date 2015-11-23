#ifndef QT_MENU_HPP
#define QT_MENU_HPP

#include "qt_connection_holder.hpp"
#include "core_ui_framework/i_menu.hpp"

#include <string>

class QAction;
class QMenu;

typedef std::map< IAction *, QAction * > Actions;

class QtMenu : public IMenu
{
public:
	QtMenu( QObject & menu, const char * windowId );

	const char * path() const override;
	const char * windowId() const override;

	void update() override;
	
	const char * relativePath( const char * path ) const;

	QAction * createQAction( IAction & action );
	void destroyQAction( IAction & action );
	QAction * getQAction( IAction & action );

	const Actions& getActions() const;

	static void addMenuAction( QMenu & qMenu, QAction & qAction, const char * path );
	static void removeMenuAction( QMenu & qMenu, QAction & qAction, const char * path );

private:
	QObject & menu_;
	Actions actions_;
	
	std::string path_;
	std::string windowId_;
	QtConnectionHolder connections_;
};

#endif//QT_MENU_BAR_HPP