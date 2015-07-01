#ifndef MAYA_MENU_BAR_HPP
#define MAYA_MENU_BAR_HPP

#include "maya_menu.hpp"
#include <map>
#include <QtCore/QString>

class QMenu;
class QMenuBar;

class MayaMenuBar : public MayaMenu
{
public:
	MayaMenuBar( QMenuBar & qMenuBar );

	void addAction( IAction & action, const char * path ) override;
	void removeAction( IAction & action ) override;

private:
	QMenuBar & qMenuBar_;

	typedef std::map< QString, QMenu * > MenuMap;
	std::map< QMenu *, MenuMap > subMenus_;
};

#endif//MAYA_MENU_BAR_HPP