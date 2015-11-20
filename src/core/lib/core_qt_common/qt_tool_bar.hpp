#ifndef QT_TOOL_BAR_HPP
#define QT_TOOL_BAR_HPP

#include "qt_menu.hpp"

class QToolBar;

class QtToolBar : public QtMenu
{
public:
	QtToolBar( QToolBar & qToolBar, const char * windowId );

	void addAction( IAction & action ) override;
	void removeAction( IAction & action ) override;

private:
	QToolBar & qToolBar_;
};

#endif//QT_TOOL_BAR_HPP