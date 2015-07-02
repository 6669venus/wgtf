#include "maya_tool_bar.hpp"

#include <QToolBar>

MayaToolBar::MayaToolBar( QToolBar & qToolBar )
	: MayaMenu( qToolBar )
	, qToolBar_( qToolBar )
{
	qToolBar_.setVisible( false );
}

void MayaToolBar::addAction( IAction & action, const char * path )
{
	auto qAction = createQAction( action );

	// TODO: deal with nested tool bars
	qToolBar_.addAction( qAction );

	qToolBar_.setVisible( true );
}

void MayaToolBar::removeAction( IAction & action )
{
	// TODO
}