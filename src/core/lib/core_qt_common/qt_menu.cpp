#include "qt_menu.hpp"

#include "core_ui_framework/i_action.hpp"
#include "core_logging/logging.hpp"
#include <QAction>
#include <QObject>
#include <QString>

namespace QtMenu_Locals
{
	const char * modeDisabled = "_disabled";
	const char * modeActive = "_active";
	const char * modeSelected = "_selected";
	const char * stateOn = "_checked";

	QIcon generateIcon( IAction & action )
	{
		QIcon icon;

		QString normalPath( action.icon() );
		QString disabledPath( normalPath + modeDisabled );
		QString activePath( normalPath + modeActive );
		QString selectedPath( normalPath + modeSelected );
		QString normalOnPath( normalPath + stateOn );
		QString disabledOnPath( normalOnPath + modeDisabled );
		QString activeOnPath( normalOnPath + modeActive );
		QString selectedOnPath( normalOnPath + modeSelected );

		icon.addPixmap( QPixmap( normalPath ), QIcon::Normal, QIcon::Off );
		icon.addPixmap( QPixmap( disabledPath ), QIcon::Disabled, QIcon::Off );
		icon.addPixmap( QPixmap( activePath ), QIcon::Active, QIcon::Off );
		icon.addPixmap( QPixmap( selectedPath ), QIcon::Selected, QIcon::Off );
		icon.addPixmap( QPixmap( normalOnPath ), QIcon::Normal, QIcon::On );
		icon.addPixmap( QPixmap( disabledOnPath ), QIcon::Disabled, QIcon::On );
		icon.addPixmap( QPixmap( activeOnPath ), QIcon::Active, QIcon::On );
		icon.addPixmap( QPixmap( selectedOnPath ), QIcon::Selected, QIcon::On );

		return icon;
	}
}

QtMenu::QtMenu( QObject & menu, const char * windowId )
	: menu_( menu )
	, windowId_( windowId )
{
	auto pathProperty = menu_.property( "path" );
	if (pathProperty.isValid())
	{
		path_ = pathProperty.toString().toUtf8().operator const char *();
	}
}

const char * QtMenu::path() const
{
	return path_.c_str();
}

const char * QtMenu::windowId() const
{
	return windowId_.c_str();
}

void QtMenu::update()
{
	for (auto & action : actions_)
	{
		action.second->setEnabled( action.first->enabled() );
		if (action.second->isCheckable())
		{
			action.second->setChecked( action.first->checked() );
		}
		
	}
}

QAction * QtMenu::createQAction( IAction & action )
{
	auto qAction = getQAction( action );
	if (qAction != nullptr)
	{
		NGT_WARNING_MSG("Action %s already existing.\n", action.text());
		return nullptr;
	}

	qAction = new QAction( action.text(), &menu_ );
	actions_[ &action ] = qAction;

	qAction->setIcon( QtMenu_Locals::generateIcon( action ) );
	qAction->setShortcut( QKeySequence( action.shortcut() ) );
	qAction->setEnabled( action.enabled() );
	if (action.isCheckable())
	{
		qAction->setCheckable( true );
		qAction->setChecked( action.checked() );
	}
	
	connections_ += QObject::connect( qAction, &QAction::triggered, [&action] () { action.execute(); } );

	return qAction;
}

QAction * QtMenu::getQAction( IAction & action )
{
	auto it = actions_.find( &action );
	if (it != actions_.end())
	{
		return it->second;
	}
	return nullptr;
}

const Actions& QtMenu::getActions() const
{
	return actions_;
}
