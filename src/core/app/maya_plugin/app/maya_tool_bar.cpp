#include "maya_tool_bar.hpp"

#include <QToolBar>

MayaToolBar::MayaToolBar( QToolBar & qToolBar )
	: MayaMenu( qToolBar )
	, qToolBar_( qToolBar )
{
}

void MayaToolBar::addAction( IAction & action, const char * path )
{
	auto qAction = createQAction( action );

	// TODO: deal with nested tool bars
	qToolBar_.addAction( qAction );
}

void MayaToolBar::removeAction( IAction & action )
{
	// TODO
}