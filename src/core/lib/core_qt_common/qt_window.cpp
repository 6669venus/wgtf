#include "qt_window.hpp"
#include "qt_dock_region.hpp"
#include "qt_menu_bar.hpp"
#include "qt_tab_region.hpp"
#include "qt_tool_bar.hpp"
#include "i_qt_framework.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_view.hpp"

#include <cassert>

#include <QDockWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QTabWidget>
#include <QToolBar>
#include <QUiLoader>
#include <QMainWindow>

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

QtWindow::QtWindow( IQtFramework & qtFramework, QIODevice & source )
	: qtFramework_( qtFramework )
{
	QUiLoader loader;

	// Load optional plugins that may have custom widgets
	auto& pluginPath = qtFramework.getPluginPath();
	if( !pluginPath.empty() )
		loader.addPluginPath( pluginPath.c_str() );
	
	auto qMainWindow = qobject_cast< QMainWindow * >( loader.load( &source ) );

	if (qMainWindow == nullptr)
	{
		return;
	}

	mainWindow_.reset( qMainWindow );

	auto idProperty = mainWindow_->property( "id" );
	if (idProperty.isValid())
	{
		id_ = idProperty.toString().toUtf8().operator const char *();
	}

	auto menuBars = getChildren< QMenuBar >( *mainWindow_ );
	for (auto & menuBar : menuBars)
	{
		if (menuBar->property( "path" ).isValid())
		{
			menus_.emplace_back( new QtMenuBar( *menuBar ) );
		}
	}

	auto toolBars = getChildren< QToolBar >( *mainWindow_ );
	for (auto & toolBar : toolBars)
	{
		if (toolBar->property( "path" ).isValid())
		{
			menus_.emplace_back( new QtToolBar( *toolBar ) );
		}
	}

	auto dockWidgets = getChildren< QDockWidget >( *mainWindow_ );
	for (auto & dockWidget : dockWidgets)
	{
		if ( dockWidget->property( "layoutTags" ).isValid() )
		{
			regions_.emplace_back( new QtDockRegion( qtFramework_, *mainWindow_, *dockWidget ) );
		}
	}

	auto tabWidgets = getChildren< QTabWidget >( *mainWindow_ );
	for (auto & tabWidget : tabWidgets)
	{
		if ( tabWidget->property( "layoutTags" ).isValid() )
		{
			regions_.emplace_back( new QtTabRegion( qtFramework_, *tabWidget ) );
		}
	}
	modalityFlag_ = mainWindow_->windowModality();
}

QtWindow::~QtWindow()
{

}

const char * QtWindow::id() const
{
	return id_.c_str();
}

const char * QtWindow::title() const
{
	if (mainWindow_.get() == nullptr)
	{
		return "";
	}

	return mainWindow_->windowTitle().toUtf8().constData();
}

void QtWindow::update()
{
	for (auto & menu : menus_)
	{
		menu->update();
	}
}

void QtWindow::close()
{
	if (mainWindow_.get() == nullptr)
	{
		return;
	}

	mainWindow_->close();
}

void QtWindow::show()
{
	if (mainWindow_.get() == nullptr)
	{
		return;
	}
	mainWindow_->setWindowModality( modalityFlag_ );
	mainWindow_->show();
}

void QtWindow::showModal()
{
	if (mainWindow_.get() == nullptr)
	{
		return;
	}
	mainWindow_->setWindowModality( Qt::ApplicationModal );
	mainWindow_->show();
}

void QtWindow::hide()
{
	if (mainWindow_.get() == nullptr)
	{
		return;
	}

	mainWindow_->hide();
}

const Menus & QtWindow::menus() const
{
	return menus_;
}

const Regions & QtWindow::regions() const
{
	return regions_;
}

QMainWindow * QtWindow::window() const
{
	return mainWindow_.get();
}
