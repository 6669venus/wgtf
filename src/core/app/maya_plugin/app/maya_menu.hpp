#ifndef MAYA_MENU_HPP
#define MAYA_MENU_HPP

#include "core_ui_framework/i_menu.hpp"
#include <QtCore/QObject>
#include <string>
#include <map>

class QAction;
class QTimer;

class MayaMenu : public QObject, public IMenu
{
	Q_OBJECT
public:
	MayaMenu( QObject & menu );

	const char * path() const override;

	void update() override;
	
	QAction * createQAction( IAction & action );

public slots:
	void executeAction();

private:
	QObject & menu_;
	std::map< IAction *, QAction * > actions_;
	std::string path_;
};

#endif//MAYA_MENU_HPP