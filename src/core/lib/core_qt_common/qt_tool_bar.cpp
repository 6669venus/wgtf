#include "qt_tool_bar.hpp"

#include <QToolBar>

QtToolBar::QtToolBar( QToolBar & qToolBar, const char * windowId )
	: QtMenu( qToolBar, windowId )
	, qToolBar_( qToolBar )
{
	qToolBar_.setVisible( false );
}

void QtToolBar::addAction( IAction & action, const char * path )
{
	auto qAction = createQAction( action );

	// TODO: deal with nested tool bars
	qToolBar_.addAction( qAction );

	qToolBar_.setVisible( true );
}

void QtToolBar::removeAction( IAction & action )
{
	// TODO
}