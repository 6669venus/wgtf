#include "qt_window.hpp"
#include "automation/interfaces/automation_interface.hpp"
#include "i_qt_framework.hpp"
#include "ui_framework/i_action.hpp"
#include "ui_framework/i_view.hpp"

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
				auto layoutHintProperty = childT->property( "layoutHint" );
				auto pathProperty = childT->property( "path" );
				if (layoutHintProperty.isValid() ||
					pathProperty.isValid())
				{
					children.push_back( childT );
				}
			}
			auto grandChildren = getChildren< T >( *child );
			children.insert( 
				children.end(), grandChildren.begin(), grandChildren.end() );
		}
		return children;
	}

	template< typename T >
	void hideWidgets( const std::vector< T * > & widgets )
	{
		for ( auto & widget : widgets )
		{
			widget->setVisible( false );
		}
	}

	template< typename T >
	bool matchWidget( const std::vector< T * > & widgets, const QString & path,
		QObject *& o_MatchedWidget, QString & o_MatchedPath )
	{
		bool matched = false;
		for ( auto & widget : widgets )
		{
			QString testPath = "";

			auto pathProperty = widget->property( "path" );
			if (pathProperty.isValid())
			{
				testPath = pathProperty.toString();
			}

			if (testPath.length() <= o_MatchedPath.length())
			{
				if (testPath.isEmpty() && o_MatchedWidget == nullptr)
				{
					o_MatchedPath = testPath;
					o_MatchedWidget = widget;
					matched = true;
				}
				continue;
			}

			if (path == testPath ||
				( path.startsWith( testPath, Qt::CaseInsensitive ) &&
				path.at( testPath.length() ) == '.' ))
			{
				o_MatchedPath = testPath;
				o_MatchedWidget = widget;
				matched = true;
			}
		}
		return matched;
	}
}

QtWindow::QtWindow( IQtFramework & qtFramework, QIODevice & source )
	: qtFramework_( qtFramework )
{
	QUiLoader loader;
	auto qMainWindow = qobject_cast< QMainWindow * >( loader.load( &source ) );
	if (qMainWindow == nullptr)
	{
		return;
	}

	auto idProperty = qMainWindow->property( "id" );
	if (idProperty.isValid())
	{
		id_ = idProperty.toString().toUtf8();
	}

	mainWindow_.reset( qMainWindow );
	menuBars_ = getChildren< QMenuBar >( *mainWindow_ );
	toolBars_ = getChildren< QToolBar >( *mainWindow_ );
	panelRegions_ = getChildren< QDockWidget >( *mainWindow_ );
	frameRegions_ = getChildren< QTabWidget >( *mainWindow_ );

	hideWidgets( menuBars_ );
	hideWidgets( toolBars_ );
	hideWidgets( panelRegions_ );
	hideWidgets( frameRegions_ );
}

QtWindow::~QtWindow()
{

}

const char * QtWindow::id()
{
	return id_.c_str();
}

void QtWindow::update()
{
	for (auto & action : actions_)
	{
		auto iAction = action.first;
		auto qAction = action.second;
		qAction->setEnabled( iAction->enabled() );
	}

	for (auto & panel : panels_)
	{
		auto iPanel = panel.first;
		iPanel->update();
	}

	for (auto & frame : frames_)
	{
		auto iFrame = frame.first;
		iFrame->update();
	}

	AutomationInterface* pAutomation =
		Context::queryInterface< AutomationInterface >();
	if (pAutomation)
	{
		if (pAutomation->timedOut())
		{
			this->close();
		}
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

void QtWindow::addFrame( IView & frame, LayoutHint & hint )
{
	if (frames_.find( &frame ) != frames_.end())
	{
		return;
	}

	auto qWidget = qtFramework_.toQWidget( frame );
	if (qWidget == nullptr)
	{
		return;
	}

	frames_[ &frame ] = qWidget;

	if (frameRegions_.empty())
	{
		return;
	}

	auto frameRegion = frameRegions_[0];
	frameRegion->setVisible( true );
	frameRegion->addTab( qWidget, frame.title() );
}

void QtWindow::addPanel( IView & panel, LayoutHint & hint )
{
	if (panels_.find( &panel ) != panels_.end())
	{
		return;
	}

	auto qWidget = qtFramework_.toQWidget( panel );
	if (qWidget == nullptr)
	{
		return;
	}

	auto qDockWidget = new QDockWidget( panel.title() );
	qDockWidget->setWidget( qWidget );
	panels_[ &panel ] = qDockWidget;

	if (panelRegions_.empty())
	{
		return;
	}

	auto panelRegion = panelRegions_[0];
	mainWindow_->tabifyDockWidget( panelRegion, qDockWidget );
}

void QtWindow::addAction( IAction & action, const char * path, const char * shortcut )
{
	if (actions_.find( &action ) != actions_.end())
	{
		return;
	}

	auto qAction = new QAction( action.text(), mainWindow_.get() );
	// Temporary: shortcuts are to be data driven
	if (shortcut != nullptr)
	{
		qAction->setShortcut( QKeySequence( shortcut ) );
	}
	connections_ += QObject::connect( 
		qAction, &QAction::triggered, [&] () { action.execute(); } );
	actions_[ &action ] = qAction;

	QString matchedPath = "";
	QObject * matchedObject = nullptr;

	bool matchedMenuBar = 
		matchWidget( menuBars_, path, matchedObject, matchedPath );
	bool matchedToolBar =
		matchWidget( toolBars_, path, matchedObject, matchedPath );

	if (matchedToolBar)
	{
		auto toolBar = qobject_cast< QToolBar * >( matchedObject );
		assert( toolBar != nullptr );

		QString subPath = matchedPath.isEmpty() ? path : 
			path + matchedPath.length();
		auto subPaths = subPath.split( '.', QString::SkipEmptyParts );

		// TODO: How to deal with sub paths greater than depth of 1
		toolBar->addAction( qAction );
		toolBar->setVisible( true );
	}
	else if (matchedMenuBar)
	{
		auto menuBar = qobject_cast< QMenuBar * >( matchedObject );
		assert( menuBar != nullptr );

		QString subPath = matchedPath.isEmpty() ? path : 
			path + matchedPath.length();
		auto subPaths = subPath.split( '.', QString::SkipEmptyParts );

		if (subPaths.size() == 0)
		{
			menuBar->addAction( qAction );
		}
		else 
		{
			QMenu * menu = nullptr;
			while (!subPaths.empty())
			{
				menu = ( menu == nullptr ? menuBar->addMenu( subPaths.first() ) :
					menu->addMenu( subPaths.first() ) );
				subPaths.pop_front();
			}
			assert( menu != nullptr );
			menu->addAction( qAction );
		}
		menuBar->setVisible( true );
	}
}

QMainWindow * QtWindow::window() const
{
	return mainWindow_.get();
}