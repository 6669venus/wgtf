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

void QtContextMenu::addAction( IAction & action, const char * path )
{
	auto qAction = createQAction( action );
	if (qAction == nullptr)
	{
		return;
	}
	
	if (path == nullptr || *path == 0)
	{
		path = action.path();
	}
	
	QMenu * menu = &qMenu_;
	while (path != nullptr && ( strcmp( this->path(), path ) != 0 ) )
	{
		auto tok = strchr( path, '.' );
		auto subPath = tok != nullptr  ? QString::fromUtf8( path, tok - path ) : path;
		if (!subPath.isEmpty())
		{
			QMenu * subMenu = menu->findChild<QMenu*>( subPath, Qt::FindDirectChildrenOnly );

			if (subMenu == nullptr)
			{
				subMenu = menu->addMenu( subPath );
				subMenu->setObjectName( subPath );
			}
			menu = subMenu;
		}
		path = tok != nullptr ? tok + 1 : nullptr;
	}

	assert( menu != nullptr );
	menu->addAction( qAction );
	qView_.addAction( qAction );
	qMenu_.repaint();
}

void QtContextMenu::removeAction( IAction & action )
{
	auto qAction = getQAction( action );
	if (qAction == nullptr)
	{
		NGT_ERROR_MSG("Target action %s %s does not exist\n", action.text(), action.path());
		return;
	}
	
	QMenu * menu = &qMenu_;
	const char * path = action.path();
	if (path == nullptr || *path == 0)
	{
		path = action.text();
	}

	while (path != nullptr)
	{
		auto tok = strchr( path, '.' );
		auto subPath = tok != nullptr  ? QString::fromUtf8( path, tok - path ) : path;
		if (!subPath.isEmpty())
		{
			QMenu * subMenu = menu->findChild<QMenu*>( subPath, Qt::FindDirectChildrenOnly );
			if (subMenu == nullptr)
			{
				NGT_ERROR_MSG("Invalid menu path token %s\n", subPath.toLatin1().data());
			}
			menu = subMenu;
		}
		path = tok != nullptr ? tok + 1 : nullptr;
	}

	assert(menu != nullptr);
	
	qView_.removeAction( qAction );
	menu->removeAction( qAction );
	if (menu->isEmpty())
	{
		menu->setParent( nullptr );
		delete menu;
		menu = nullptr;
	}

	qAction->setParent( nullptr );
	actions_.erase( &action );
	delete qAction;
	qAction = nullptr;
}
