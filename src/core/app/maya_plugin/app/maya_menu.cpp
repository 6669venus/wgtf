#include "maya_menu.hpp"

#include "ui_framework/i_action.hpp"

#include <QtGui/QAction>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QTimer>

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

MayaMenu::MayaMenu( QObject & menu )
	: menu_( menu )
{
	auto pathProperty = menu_.property( "path" );
	if (pathProperty.isValid())
	{
		path_ = pathProperty.toString().toUtf8();
	}
}

const char * MayaMenu::path() const
{
	return path_.c_str();
}

void MayaMenu::update()
{
	for (auto & action : actions_)
	{
		action.second->setEnabled( action.first->enabled() );
	}
}

QAction * MayaMenu::createQAction( IAction & action )
{
	auto it = actions_.find( &action );
	if (it != actions_.end())
	{
		//TODO error?
		return nullptr;
	}

	auto qAction = new QAction( action.text(), &menu_ );
	actions_[ &action ] = qAction;

	qAction->setIcon( QtMenu_Locals::generateIcon( action ) );
	qAction->setShortcut( QKeySequence( action.shortcut() ) );
	qAction->setEnabled( action.enabled() );
	/*connections_ += */QObject::connect( qAction, SIGNAL(QAction::triggered()), 
		this, SLOT(MayaMenu::executeAction()) );

	return qAction;
}

void MayaMenu::executeAction()
{
	QAction* qAction = qobject_cast<QAction*>( sender() );
	
	//FIXME: low performance
	for ( auto iter = actions_.begin(); iter != actions_.end(); ++iter )
	{
		if ( iter->second == qAction )
		{
			iter->first->execute();
			break;
		}
	}
}
