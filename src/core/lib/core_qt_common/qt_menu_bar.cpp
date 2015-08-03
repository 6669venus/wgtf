#include "qt_menu_bar.hpp"

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
	// TODO
}