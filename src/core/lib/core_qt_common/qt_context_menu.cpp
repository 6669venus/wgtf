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

	QColor highlightShade = qMenu_.palette().color(QPalette::Highlight);
	QColor textShade = qMenu_.palette().color(QPalette::Disabled, QPalette::Text);
	
	QString menuStyleSheet("QMenu {\n    background-color: palette(window);\n    margin: 2px;\n}\n\nQMenu::item {\n    padding: 2px 25px 2px 20px;\n    border: 1px solid transparent;\n}\n\nQMenu::item:selected {\n    border-color: palette(highlight);\n    background: rgba(%1, %2, %3, 128);\n	color: palette(highlighted-text);\n}\n\nQMenu::item:disabled {\n	color: rgba(%4, %5, %6, %7);\n}\n\nQMenu::separator {\n    height: 1px;\n    background: palette(dark);\n    margin-left: 10px;\n    margin-right: 5px;\n}\n\nQMenu::indicator {\n    width: 13px;\n    height: 13px;\n}");

	qMenu_.setStyleSheet(menuStyleSheet.arg(highlightShade.red()).arg(highlightShade.green()).arg(highlightShade.blue()).arg(textShade.red()).arg(textShade.green()).arg(textShade.blue()).arg(textShade.alpha()));

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
