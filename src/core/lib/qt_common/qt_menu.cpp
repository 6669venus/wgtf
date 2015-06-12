#include "qt_menu.hpp"

#include "ui_framework/i_action.hpp"

#include <QAction>
#include <QObject>
#include <QString>
#include <QTimer>

QtMenu::QtMenu( QObject & menu )
	: menu_( menu )
{
	timer_ = new QTimer( &menu_ );
	timer_->start();

	auto pathProperty = menu_.property( "path" );
	if (pathProperty.isValid())
	{
		path_ = pathProperty.toString().toUtf8();
	}
}

const char * QtMenu::path() const
{
	return path_.c_str();
}

QAction * QtMenu::createQAction( IAction & action )
{
	auto qAction = new QAction( action.text(), &menu_ );
	qAction->setShortcut( QKeySequence( action.shortcut() ) );
	qAction->setEnabled( action.enabled() );
	connections_ += QObject::connect( qAction, &QAction::triggered, 
		[&action] () { action.execute(); } );
	connections_ += QObject::connect( timer_, &QTimer::timeout, 
		[qAction, &action]() { qAction->setEnabled( action.enabled() ); } );

	return qAction;
}