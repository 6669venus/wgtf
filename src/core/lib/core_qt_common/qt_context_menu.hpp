#ifndef QT_CONTEXT_MENU
#define QT_CONTEXT_MENU

#include "qt_menu.hpp"

class QMenu;

class QtContextMenu : public QtMenu
{
public:
	QtContextMenu( QMenu & qMenu );

	void addAction( IAction & action, const char * path ) override;
	void removeAction( IAction & action ) override;

private:
	QMenu & qMenu_;
};

#endif // QT_CONTEXT_MENU