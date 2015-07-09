#ifndef MAYA_TOOL_BAR_HPP
#define MAYA_TOOL_BAR_HPP

#include "maya_menu.hpp"

class QToolBar;

class MayaToolBar : public MayaMenu
{
public:
	MayaToolBar( QToolBar & qToolBar );

	void addAction( IAction & action, const char * path ) override;
	void removeAction( IAction & action ) override;

private:
	QToolBar & qToolBar_;
};

#endif//MAYA_TOOL_BAR_HPP