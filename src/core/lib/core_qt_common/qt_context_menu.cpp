#include "qt_context_menu.hpp"

#include "core_ui_framework/i_action.hpp"
#include "core_logging/logging.hpp"
#include "core_string_utils/string_utils.hpp"

#include <QMenu>
#include <assert.h>
#include <sstream>

QtContextMenu::QtContextMenu( QMenu & qMenu, QWidget * qView, const char * windowId )
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
		if(qAction == nullptr)
		{
			return;
		}
		qAction->setShortcutContext(Qt::WidgetShortcut);
	}
	
	QtMenu::addMenuAction( qMenu_, *qAction, relativePath( path ) );
	
	if (qView_ != nullptr)
	{
		qView_->addAction(qAction);
	}
}

void QtContextMenu::removeAction( IAction & action )
{
	auto qAction = getQAction( action );
	if (qAction == nullptr)
	{
		NGT_ERROR_MSG("Target action '%s' '%s' does not exist\n", action.text(), StringUtils::join(action.paths(), ';').c_str());
		return;
	}

	if (qView_ != nullptr)
	{
		qView_->removeAction( qAction );
	}

	QtMenu::removeMenuAction( qMenu_, *qAction );

	destroyQAction( action );
}
