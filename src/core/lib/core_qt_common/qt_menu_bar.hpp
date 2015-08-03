#ifndef QT_MENU_BAR_HPP
#define QT_MENU_BAR_HPP

#include "qt_menu.hpp"

class QMenu;
class QMenuBar;

class QtMenuBar : public QtMenu
{
public:
	QtMenuBar( QMenuBar & qMenuBar );

	void addAction( IAction & action, const char * path ) override;
	void removeAction( IAction & action ) override;

private:
	QMenuBar & qMenuBar_;

	typedef std::map< QString, QMenu * > MenuMap;
	std::map< QMenu *, MenuMap > subMenus_;
};

#endif//QT_MENU_BAR_HPP