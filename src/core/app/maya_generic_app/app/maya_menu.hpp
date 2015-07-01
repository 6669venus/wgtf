#ifndef MAYA_MENU_HPP
#define MAYA_MENU_HPP

#include "qt_connection_holder.hpp"
#include "ui_framework/i_menu.hpp"

#include <string>

class QAction;
class QTimer;

class MayaMenu : public IMenu
{
public:
	MayaMenu( QObject & menu );

	const char * path() const override;

	void update() override;
	
	QAction * createQAction( IAction & action );

private:
	QObject & menu_;
	std::map< IAction *, QAction * > actions_;
	std::string path_;
	QtConnectionHolder connections_;
};

#endif//MAYA_MENU_HPP