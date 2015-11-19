#include "qt_tool_bar.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_logging/logging.hpp"
#include <QToolBar>

QtToolBar::QtToolBar( QToolBar & qToolBar, const char * windowId )
	: QtMenu( qToolBar, windowId )
	, qToolBar_( qToolBar )
{
	qToolBar_.setVisible( false );
}

void QtToolBar::addAction( IAction & action )
{
	auto qAction = createQAction( action );

	// TODO: deal with nested tool bars
	qToolBar_.addAction( qAction );

	qToolBar_.setVisible( true );
}

void QtToolBar::removeAction( IAction & action )
{
	auto qAction = createQAction( action );
	if (qAction == nullptr)
	{
		NGT_ERROR_MSG("Target action %s %s does not exist\n", action.text(), action.path());
		return;
	}

	qToolBar_.removeAction( qAction );

	destroyQAction( action );
}