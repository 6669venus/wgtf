#ifndef QT_MENU_HPP
#define QT_MENU_HPP

#include "qt_connection_holder.hpp"
#include "core_ui_framework/i_menu.hpp"

#include <string>

class QAction;

class QtMenu : public IMenu
{
public:
	QtMenu( QObject & menu );

	const char * path() const override;

	void update() override;
	
	QAction * createQAction( IAction & action );

private:
	QObject & menu_;
	std::map< IAction *, QAction * > actions_;
	std::string path_;
	QtConnectionHolder connections_;
};

#endif//QT_MENU_BAR_HPP