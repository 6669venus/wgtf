#include "qt_context_menu.hpp"

#include "core_ui_framework/i_action.hpp"
#include "core_logging/logging.hpp"

#include <QMenu>
#include <assert.h>

QtContextMenu::QtContextMenu( QMenu & qMenu, QWidget & qView, const char * windowId )
	: QtMenu( qMenu, windowId )
	, qMenu_( qMenu )
	, qView_( qView )
{
}

void QtContextMenu::addAction( IAction & action )
{
	auto qAction = createQAction( action );
	if (qAction == nullptr)
	{
		return;
	}
	
	QtMenu::addMenuAction( qMenu_, *qAction, relativePath( action.path() ) );
	qView_.addAction( qAction );
}

void QtContextMenu::removeAction( IAction & action )
{
	auto qAction = getQAction( action );
	if (qAction == nullptr)
	{
		NGT_ERROR_MSG("Target action %s %s does not exist\n", action.text(), action.path());
		return;
	}

	qView_.removeAction( qAction );
	QtMenu::removeMenuAction( qMenu_, *qAction, relativePath( action.path() ) );

	destroyQAction( action );
}
