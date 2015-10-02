#include "qt_menu_bar.hpp"
#include "core_ui_framework/i_action.hpp"
#include <QMenuBar>

QtMenuBar::QtMenuBar( QMenuBar & qMenuBar )
	: QtMenu( qMenuBar )
	, qMenuBar_( qMenuBar )
{
	qMenuBar_.setVisible( false );
}

void QtMenuBar::addAction( IAction & action, const char * path )
{
	auto qAction = createQAction( action );
	if (qAction == nullptr)
	{
		return;
	}
	QMenu * menu = nullptr;
	while (path != nullptr)
	{
		auto tok = strchr( path, '.' );
		auto subPath = tok != nullptr  ? QString::fromUtf8( path, tok - path ) : path;
		if (!subPath.isEmpty())
		{
			auto it = subMenus_.find( menu );
			if (it == subMenus_.end())
			{
				it = subMenus_.insert( std::make_pair( menu, MenuMap() ) ).first;
			}

			auto subMenuIt = it->second.find( subPath );
			if (subMenuIt == it->second.end())
			{
				if (menu == nullptr)
				{
					menu = qMenuBar_.addMenu( subPath );
				}
				else
				{
					menu = menu->addMenu( subPath );
				}
				it->second.insert( std::make_pair( subPath, menu ) );
			}
			else
			{
				menu = subMenuIt->second;
			}
		}
		path = tok != nullptr ? tok + 1 : nullptr;
	}
	menu == nullptr ? qMenuBar_.addAction( qAction ) : menu->addAction( qAction );

	qMenuBar_.setVisible( true );
}

void QtMenuBar::removeAction( IAction & action )
{
	QMenu * menu = nullptr;
	const char * path = action.path();
	
	while (path != nullptr)
	{
		auto tok = strchr( path, '.' );
		auto subPath = tok != nullptr  ? QString::fromUtf8( path, tok - path ) : path;
		if (!subPath.isEmpty())
		{
			auto it = subMenus_.find( menu );
			if (it != subMenus_.end())
			{
				auto subMenuIt = it->second.find( subPath );
				if (subMenuIt != it->second.end())
				{
					menu = subMenuIt->second;
					it->second.erase( subPath );
				}
			}
		}
		path = tok != nullptr ? tok + 1 : nullptr;
	}

	if (menu != nullptr)
	{
		menu->setParent( nullptr );
		delete menu;
		menu = nullptr;
	}
	auto qAction = getQAction( action );
	if (qAction == nullptr)
	{
		return;
	}
	qMenuBar_.removeAction( qAction );
	qAction->setParent( nullptr );
	actions_.erase( &action );
	delete qAction;
	qAction = nullptr;
}