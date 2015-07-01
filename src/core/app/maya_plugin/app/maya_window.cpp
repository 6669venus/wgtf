#include "maya_window.hpp"
#include "maya_dock_region.hpp"
#include "maya_menu_bar.hpp"
#include "maya_tab_region.hpp"
#include "maya_tool_bar.hpp"

#include "qt_common/i_qt_framework.hpp"
#include "ui_framework/i_action.hpp"
#include "ui_framework/i_view.hpp"

#include <maya/MQtUtil.h>

#include <QtGui/QDockWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QTabWidget>
#include <QtGui/QToolBar>
#include <QtGui/QMainWindow>

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

MayaWindow::MayaWindow( IQtFramework & qtFramework )
	: qtFramework_( qtFramework )
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
		if (menuBar->property( "path" ).isValid())
		{
			menus_.emplace_back( new MayaMenuBar( *menuBar ) );
		}
	}

	auto toolBars = getChildren< QToolBar >( *mainWindow_ );
	for (auto & toolBar : toolBars)
	{
		if (toolBar->property( "path" ).isValid())
		{
			menus_.emplace_back( new MayaToolBar( *toolBar ) );
		}
	}

	auto dockWidgets = getChildren< QDockWidget >( *mainWindow_ );
	for (auto & dockWidget : dockWidgets)
	{
		if ( dockWidget->property( "layoutTags" ).isValid() )
		{
			regions_.emplace_back(
				new MayaDockRegion( qtFramework_,
					*mainWindow_, *dockWidget ) );
		}
	}

	auto tabWidgets = getChildren< QTabWidget >( *mainWindow_ );
	for (auto & tabWidget : tabWidgets)
	{
		if ( tabWidget->property( "layoutTags" ).isValid() )
		{
			regions_.emplace_back(
				new MayaTabRegion( qtFramework_, *tabWidget ) );
		}
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