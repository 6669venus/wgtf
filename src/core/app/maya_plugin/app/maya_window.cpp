#include "maya_window.hpp"
#include "maya_dock_region.hpp"
#include "maya_menu_bar.hpp"
#include "maya_tab_region.hpp"
#include "maya_tool_bar.hpp"

#include "ui_framework/i_action.hpp"
#include "ui_framework/i_view.hpp"


#include <QtGui/QDockWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QTabWidget>
#include <QtGui/QToolBar>
#include <QtGui/QMainWindow>

#include <maya/MQtUtil.h>

namespace
{
	template< typename T >
	std::vector< T * > getChildren( const QObject & parent )
	{
		std::vector< T * > children;
		foreach ( auto child, parent.children() )
		{
			T * childT = qobject_cast< T * >( child );
			if (childT != nullptr)
			{
				children.push_back( childT );
			}
			auto grandChildren = getChildren< T >( *child );
			children.insert( 
				children.end(), grandChildren.begin(), grandChildren.end() );
		}
		return children;
	}
}

MayaWindow::MayaWindow()
{
	mainWindow_ = qobject_cast< QMainWindow * >( MQtUtil::mainWindow() );

	if (mainWindow_ == nullptr)
	{
		return;
	}

	auto idProperty = MQtUtil::toQString( MQtUtil::fullName( mainWindow_ ) );
	if (!idProperty.isNull())
	{
		id_ = idProperty.toUtf8().constData();
	}

	auto menuBars = getChildren< QMenuBar >( *mainWindow_ );
	for (auto & menuBar : menuBars)
	{
		menus_.emplace_back( new MayaMenuBar( *menuBar ) );
	}

	auto toolBars = getChildren< QToolBar >( *mainWindow_ );
	for (auto & toolBar : toolBars)
	{
		menus_.emplace_back( new MayaToolBar( *toolBar ) );
	}

	auto dockWidgets = getChildren< QDockWidget >( *mainWindow_ );
	for (auto & dockWidget : dockWidgets)
	{
		regions_.emplace_back(
			new MayaDockRegion( *mainWindow_, *dockWidget ) );
	}

	auto tabWidgets = getChildren< QTabWidget >( *mainWindow_ );
	for (auto & tabWidget : tabWidgets)
	{
		regions_.emplace_back(
			new MayaTabRegion( *tabWidget ) );
	}
}

MayaWindow::~MayaWindow()
{

}

const char * MayaWindow::id()
{
	return id_.c_str();
}

void MayaWindow::update()
{
	for (auto & menu : menus_)
	{
		menu->update();
	}
}

void MayaWindow::close()
{
	if (mainWindow_)
	{
		mainWindow_->close();
	}
}

void MayaWindow::show()
{
	if (mainWindow_)
	{
		mainWindow_->show();
	}
}

void MayaWindow::hide()
{
	if (mainWindow_)
	{
		mainWindow_->hide();
	}
}

const Menus & MayaWindow::menus() const
{
	return menus_;
}

const Regions & MayaWindow::regions() const
{
	return regions_;
}

QMainWindow * MayaWindow::window() const
{
	return mainWindow_;
}