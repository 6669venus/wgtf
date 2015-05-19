#include "menu_helper.hpp"

#include <QMenu>
#include <QMenuBar>
#include <QMainWindow>

#include "guimanager/gui_item.hpp"
#include "guimanager/gui_manager.hpp"

//==============================================================================
MenuHelper::MenuHelper( QMainWindow * mainWindow )
	: mainWindow_( mainWindow )
	, guiManager_( NULL )
{
}

//==============================================================================
void * MenuHelper::getMenu()
{
	return mainWindow_->menuBar();
}

//==============================================================================
int MenuHelper::getMenuItemCount( void * menu )
{
	QMenuBar * menuBar = mainWindow_->menuBar();
	QList< QAction * > * actions = NULL;
	if( menu == menuBar )
	{
		actions = &menuBar->actions();
	}
	else
	{
		actions = &( ( QMenu * ) menu )->actions();
	}
	return actions->size();
}


//==============================================================================
void MenuHelper::deleteMenu( void * menu, int index )
{
	QAction * action = getMenuAction( menu, index );
	if (action == NULL)
	{
		return;
	}
	QMenuBar * menuBar = mainWindow_->menuBar();
	QList< QAction * > * actions = NULL;
	if( menu == menuBar )
	{
		menuBar->removeAction( action );
	}
	else
	{
		( ( QMenu * ) menu )->removeAction( action );
	}
}


//==============================================================================
void MenuHelper::setMenuInfo( void * menu, void * menuInfo )
{
}


//==============================================================================
void MenuHelper::destroyMenu( void * menu )
{
}


//==============================================================================
BW::GUI::Item * MenuHelper::getMenuItemInfo(
	void * menu,
	unsigned int index,
	void * typeData )
{
	QAction * action = getMenuAction( menu, index );
	if (action == NULL)
	{
		return NULL;
	}
	return static_cast< BW::GUI::Item * >( action->data().value< void * >() );
}


//==============================================================================
QAction * MenuHelper::getMenuAction( void * menu, unsigned int index )
{
	QMenuBar * menuBar = mainWindow_->menuBar();
	QList< QAction * > * actions = NULL;
	if( menu == menuBar )
	{
		actions = &menuBar->actions();
		if( index >= ( unsigned int ) actions->size() )
		{
			return NULL;
		}
		return (*actions)[ index ];
	}
	else
	{
		actions = &( ( QMenu * ) menu )->actions();
	}
	if( index >= ( unsigned int ) actions->size() )
	{
		return NULL;
	}
	return (*actions)[ index ];
}

//==============================================================================
void MenuHelper::setMenuItemInfo( void * menu, unsigned int index, void * info )
{
}


//==============================================================================
void MenuHelper::setSeperator( void * menu, unsigned int index )
{
	QAction * action = getMenuAction( menu, index );
	if (action == NULL)
	{
		return;
	}
	action->setSeparator( true );
}

//==============================================================================
void MenuHelper::insertMenuItem(
	void * menu, unsigned int index, BW::GUI::ItemPtr info )
{
	QMenuBar * menuBar = mainWindow_->menuBar();
	QAction * action;
	QList< QAction * > * actions = NULL;
	if( menu == menuBar )
	{
		action = menuBar->addMenu( new QMenu() );
		actions = &menuBar->actions();
	}
	else
	{
		QMenu * parent = ( QMenu * ) menu;
		action = new QAction( parent );
		( parent )->addAction( action );
		actions = &( parent )->actions();
	}

	actions->move( actions->indexOf( action ), index );
	BW::GUI::Item * object = info.get();
	action->setData( QVariant::fromValue( ( void * ) object ) );
	connect( action, &QAction::triggered, this, &MenuHelper::executeAction );
}

//==============================================================================
void MenuHelper::executeAction()
{
	const QAction * action = qobject_cast< const QAction * >( this->sender() );
	BW::GUI::Item * pointer
		= static_cast< BW::GUI::Item * >( action->data().value< void * >() );
	guiManager_->act( pointer->commandID() );
}

//==============================================================================
void MenuHelper::modifyMenu(
	void * menu,
	unsigned int index,
	BW::GUI::ItemPtr info,
	const wchar_t * string )
{
	QAction * action = getMenuAction( menu, index );
	if (action == NULL)
	{
		return;
	}
	action->setText( QString::fromWCharArray( string ) );
	action->setData( QVariant::fromValue( ( void * ) info.get() ) );
}


//==============================================================================
void MenuHelper::enableMenuItem(
	void * menu, unsigned int index, MenuState state )
{
	QAction * action = getMenuAction( menu, index );
	if (action == NULL)
	{
		return;
	}
	bool enabled = state & MS_ENABLED;
	action->setEnabled( enabled );
}


//==============================================================================
void MenuHelper::checkMenuItem(
	void * menu, unsigned int index, MenuCheckState state )
{
	QAction * action = getMenuAction( menu, index );
	if (action == NULL)
	{
		return;
	}
	action->setChecked( state & MCS_CHECKED );
}

//==============================================================================
void MenuHelper::updateText(
	void * hMenu, unsigned index, const std::wstring & newName )
{
	QAction * action = getMenuAction( hMenu, index );
	if (action == NULL)
	{
		return;
	}
	action->setText( QString::fromWCharArray( newName.c_str() ) );
}

//==============================================================================
void * MenuHelper::setSubMenu( void * hMenu, unsigned index )
{
	QAction * action = getMenuAction( hMenu, index );
	if (action == NULL)
	{
		insertMenuItem( hMenu, index, NULL );
		action = getMenuAction( hMenu, index );
	}
	action->setMenu( new QMenu() );
	return action->menu();
}
