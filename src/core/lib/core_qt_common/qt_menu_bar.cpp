#include "qt_menu_bar.hpp"
#include "core_ui_framework/i_action.hpp"
#include <QMenuBar>
#include <assert.h>
#include "core_logging/logging.hpp"

static const char* const DEFAULT_MENU = "Default";

QtMenuBar::QtMenuBar( QMenuBar & qMenuBar )
	: QtMenu( qMenuBar )
	, qMenuBar_( qMenuBar )
{
}

void QtMenuBar::addAction( IAction & action, const char * path )
{
	auto qAction = createQAction( action );
	if (qAction == nullptr)
	{
		return;
	}
	
	if (path == nullptr || *path == 0)
	{
		path = DEFAULT_MENU;
	}
	
	QMenu * menu = nullptr;
	while (path != nullptr)
	{
		auto tok = strchr( path, '.' );
		auto subPath = tok != nullptr  ? QString::fromUtf8( path, tok - path ) : path;
		if (!subPath.isEmpty())
		{
			QObject* m = (menu != nullptr) ? menu : static_cast<QObject*>(&qMenuBar_);
			QMenu * subMenu = m->findChild<QMenu*>( subPath, Qt::FindDirectChildrenOnly );

			if (subMenu == nullptr)
			{
				subMenu = (menu != nullptr) ? menu->addMenu( subPath ) : qMenuBar_.addMenu( subPath );
				subMenu->setObjectName( subPath );
			}
			menu = subMenu;
		}
		path = tok != nullptr ? tok + 1 : nullptr;
	}

	assert( menu != nullptr ); // evgenys: mac does not support actions as root menu items
	menu->addAction( qAction );
	qMenuBar_.repaint();
}

void QtMenuBar::removeAction( IAction & action )
{
	auto qAction = getQAction( action );
	if (qAction == nullptr)
	{
		NGT_ERROR_MSG("Target action %s %s does not exist\n", action.text(), action.path());
		return;
	}
	
	QMenu * menu = nullptr;
	const char * path = action.path();
	if (path == nullptr || *path == 0)
	{
		path = DEFAULT_MENU;
	}

	while (path != nullptr)
	{
		auto tok = strchr( path, '.' );
		auto subPath = tok != nullptr  ? QString::fromUtf8( path, tok - path ) : path;
		if (!subPath.isEmpty())
		{
			QObject* m = (menu != nullptr) ? menu : static_cast<QObject*>(&qMenuBar_);
			QMenu * subMenu = m->findChild<QMenu*>( subPath, Qt::FindDirectChildrenOnly );
			if (subMenu == nullptr)
			{
				NGT_ERROR_MSG("Invalid menu path token %s\n", subPath.toLatin1().data());
			}
			menu = subMenu;
		}
		path = tok != nullptr ? tok + 1 : nullptr;
	}

	assert(menu != nullptr);
	
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
