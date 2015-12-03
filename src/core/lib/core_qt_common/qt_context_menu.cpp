#include "qt_context_menu.hpp"

#include "core_ui_framework/i_action.hpp"
#include "core_logging/logging.hpp"
#include "core_string_utils/string_utils.hpp"

#include <QMenu>
#include <assert.h>
#include <sstream>

QtContextMenu::QtContextMenu( QMenu & qMenu, QWidget & qView, const char * windowId )
	: QtMenu( qMenu, windowId )
	, qMenu_( qMenu )
	, qView_( qView )
{
}

void QtContextMenu::addAction( IAction & action, const char* path )
{
	auto qAction = getQAction( action );
	if (qAction == nullptr)
	{
		qAction = createQAction(action);
	}
	if(qAction == nullptr)
	{
		return;
	}
	
	QtMenu::addMenuAction( qMenu_, *qAction, relativePath( path ) );
	
	qView_.addAction(qAction);
	qAction->setShortcutContext(Qt::WidgetShortcut);
}

void QtContextMenu::removeAction( IAction & action )
{
	auto qAction = getQAction( action );
	if (qAction == nullptr)
	{
		NGT_ERROR_MSG("Target action '%s' '%s' does not exist\n", action.text(), StringUtils::join(action.paths(), ';').c_str());
		return;
	}

	qView_.removeAction( qAction );

	for ( auto& path : action.paths() )
	{
		removeQAction( &qMenu_, action, qAction, relativePath( path.c_str() ) );
	}
	destroyQAction( action );
}
