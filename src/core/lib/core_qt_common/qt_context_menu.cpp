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

	//qMenu_.setStyleSheet("QMenu {\n    background-color: palette(window);\n    margin: 2px;\n}\n\nQMenu::item {\n    padding: 2px 25px 2px 20px;\n    border: 1px solid transparent;\n}\n\nQMenu::item:selected {\n    border-color: palette(highlight);\n    background: palette(highlight);\n	color: palette(highlighted-text);\n}\n\nQMenu::icon:checked {\n    background: palette(dark);\n    border: 1px inset palette(dark);\n    position: absolute;\n    top: 1px;\n    right: 1px;\n    bottom: 1px;\n    left: 1px;\n}\n\nQMenu::separator {\n    height: 1px;\n    background: palette(dark);\n    margin-left: 10px;\n    margin-right: 5px;\n}\n\nQMenu::indicator {\n    width: 13px;\n    height: 13px;\n}\n\nQMenu::indicator:non-exclusive:unchecked {\n    image: url(:/images/checkbox_unchecked.png);\n}\n\nQMenu::indicator:non-exclusive:unchecked:selected {\n    image: url(:/images/checkbox_unchecked_hover.png);\n}\n\nQMenu::indicator:non-exclusive:checked {\n    image: url(:/images/checkbox_checked.png);\n}\n\nQMenu::indicator:non-exclusive:checked:selected {\n    image: url(:/images/checkbox_checked_hover.png);\n}\n\nQMenu::indicator:exclusive:unchecked {\n    image: url(:/images/radiobutton_unchecked.png);\n}\n\nQMenu::indicator:exclusive:unchecked:selected {\n    image: url(:/images/radiobutton_unchecked_hover.png);\n}\n\nQMenu::indicator:exclusive:checked {\n    image: url(:/images/radiobutton_checked.png);\n}\n\nQMenu::indicator:exclusive:checked:selected {\n    image: url(:/images/radiobutton_checked_hover.png);\n}");

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
