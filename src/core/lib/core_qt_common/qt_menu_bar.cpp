#include "qt_menu_bar.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_logging/logging.hpp"
#include <QMenuBar>
#include <assert.h>

QtMenuBar::QtMenuBar( QMenuBar & qMenuBar, const char * windowId )
	: QtMenu( qMenuBar, windowId )
	, qMenuBar_( qMenuBar )
{
}

void QtMenuBar::addAction( IAction & action )
{
	auto qAction = createQAction( action );
	if (qAction == nullptr)
	{
		return;
	}
	
	auto path = relativePath( action.path() );
	if (path == nullptr || strlen( path ) == 0)
	{
		path = action.text();
	}

	auto tok = strchr( path, '.' );
	auto menuPath = tok != nullptr  ? QString::fromUtf8( path, tok - path ) : path;
	QMenu * menu = qMenuBar_.findChild<QMenu*>( menuPath, Qt::FindDirectChildrenOnly );
	if (menu == nullptr)
	{
		menu = qMenuBar_.addMenu( menuPath );
		menu->setObjectName( menuPath );
	}
	path = tok != nullptr ? tok + 1 : nullptr;

	QtMenu::addMenuAction( *menu, *qAction, path );
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
	
	auto path = relativePath( action.path() );
	if (path == nullptr || strlen( path ) == 0)
	{
		path = action.text();
	}

	auto tok = strchr( path, '.' );
	auto menuPath = tok != nullptr  ? QString::fromUtf8( path, tok - path ) : path;
	QMenu * menu = qMenuBar_.findChild<QMenu*>( menuPath, Qt::FindDirectChildrenOnly );
	assert (menu != nullptr);
	path = tok != nullptr ? tok + 1 : nullptr;

	QtMenu::removeMenuAction( *menu, *qAction, path );
	if (menu->isEmpty())
	{
		menu->setParent( nullptr );
		delete menu;
	}

	destroyQAction( action );
}
