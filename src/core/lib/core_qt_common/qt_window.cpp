#include "qt_window.hpp"

#include "core_reflection/property_accessor.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_view.hpp"
#include "i_qt_framework.hpp"
#include "qt_context_menu.hpp"
#include "qt_dock_region.hpp"
#include "qt_menu_bar.hpp"
#include "qt_tab_region.hpp"
#include "qt_tool_bar.hpp"
#include "core_ui_framework/i_preferences.hpp"
#include "wg_types/vector2.hpp"
#include "wg_types/binary_block.hpp"
#include <QApplication>
#include <QDockWidget>
#include <QElapsedTimer>
#include <QEvent>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QTabWidget>
#include <QToolBar>
#include <QUiLoader>
#include <QWindow>
#include <cassert>
#include <chrono>
#include <thread>

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

	const char * g_internalPreferenceId = "E28A7FA9-08D4-464F-B073-47CB9DD20F62";
}

QtWindow::QtWindow( IQtFramework & qtFramework, QIODevice & source )
	: qtFramework_( qtFramework )
	, mainWindow_( nullptr )
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
			menus_.emplace_back( new QtMenuBar( *menuBar, id_.c_str() ) );
		}
	}

	auto toolBars = getChildren< QToolBar >( *mainWindow_ );
	for (auto & toolBar : toolBars)
	{
		if (toolBar->property( "path" ).isValid())
		{
			menus_.emplace_back( new QtToolBar( *toolBar, id_.c_str() ) );
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
	mainWindow_->installEventFilter( this );
}

QtWindow::~QtWindow()
{
	if (mainWindow_ != nullptr)
	{
		this->savePreference();
		mainWindow_ = nullptr;
	}
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

void QtWindow::show( bool wait /* = false */)
{
	if (mainWindow_.get() == nullptr)
	{
		return;
	}
	mainWindow_->setWindowModality( modalityFlag_ );
	
	static bool bPreferenceLoaded = false;
	if (!bPreferenceLoaded)
	{
		if (this->loadPreference())
		{
			bPreferenceLoaded = true;
			if (wait)
			{
				waitForWindowExposed();
			}
			return;
		}
	}
	mainWindow_->show();
	if (wait)
	{
		waitForWindowExposed();
	}
}

void QtWindow::showMaximized( bool wait /* = false */)
{
	if (mainWindow_.get() == nullptr)
	{
		return;
	}
	mainWindow_->setWindowModality( modalityFlag_ );
	
	static bool bPreferenceLoaded = false;
	if (!bPreferenceLoaded)
	{
		if (this->loadPreference())
		{
			bPreferenceLoaded = true;
			if (wait)
			{
				waitForWindowExposed();
			}
			return;
		}
	}

	mainWindow_->showMaximized();
	if (wait)
	{
		waitForWindowExposed();
	}
}

void QtWindow::showModal()
{
	if (mainWindow_.get() == nullptr)
	{
		return;
	}
	mainWindow_->setWindowModality( Qt::ApplicationModal );
	static bool bPreferenceLoaded = false;
	if (!bPreferenceLoaded)
	{
		if (this->loadPreference())
		{
			bPreferenceLoaded = true;
			return;
		}
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

void QtWindow::waitForWindowExposed()
{
	if (mainWindow_.get() == nullptr)
	{
		return;
	}
	enum { TimeOutMs = 10 };
	QElapsedTimer timer;
	const int timeout = 1000;
	if (!mainWindow_->windowHandle())
	{
		mainWindow_->createWinId();
	}
	auto window = mainWindow_->windowHandle();
	timer.start();
	while (!window->isExposed()) 
	{
		const int remaining = timeout - int(timer.elapsed());
		if (remaining <= 0)
		{
			break;
		}
		QCoreApplication::processEvents(QEventLoop::AllEvents, remaining);
		QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
		std::this_thread::sleep_for( std::chrono::milliseconds( uint32_t( TimeOutMs ) ) );
	}
}

bool QtWindow::eventFilter( QObject * obj, QEvent * event )
{
	if (obj == mainWindow_.get())
	{
		if (event->type() == QEvent::Close)
		{
			this->notifyCloseEvent();
			return true;
		}
	}
	return QObject::eventFilter( obj, event );
}

void QtWindow::savePreference()
{
	auto preferences = qtFramework_.getPreferences();
	if (preferences == nullptr)
	{
		return;
	}
	std::string key = (id_ == "") ? g_internalPreferenceId : id_;
	auto preference = preferences->getPreference( key.c_str() );
	QByteArray geometryData = mainWindow_->saveGeometry();
	QByteArray layoutData = mainWindow_->saveState();
	std::shared_ptr< BinaryBlock > geometry = 
		std::make_shared< BinaryBlock >(geometryData.constData(), geometryData.size(), false );
	std::shared_ptr< BinaryBlock > state = 
		std::make_shared< BinaryBlock >(layoutData.constData(), layoutData.size(), false );
	preference->set( "geometry", geometry );
	preference->set( "layoutState", state );
	bool isMaximized = mainWindow_->isMaximized();
	preference->set( "maximized", isMaximized );
	if (!isMaximized)
	{
		auto pos = mainWindow_->pos();
		auto size = mainWindow_->size();
		preference->set( "pos",Vector2( pos.x(), pos.y() ) );
		preference->set( "size",Vector2( size.width(), size.height() ) );
	}
}

bool QtWindow::loadPreference()
{
	auto preferences = qtFramework_.getPreferences();
	if (preferences == nullptr)
	{
		return false;
	}
	std::string key = (id_ == "") ? g_internalPreferenceId : id_;
	auto preference = preferences->getPreference( key.c_str() );
	auto accessor = preference->findProperty( "geometry" );
	if (accessor.isValid())
	{
		std::shared_ptr< BinaryBlock > geometry;
		bool isOk = preference->get( "geometry", geometry );
		assert( isOk );
		isOk = mainWindow_->restoreGeometry( QByteArray( geometry->cdata(), static_cast<int>(geometry->length()) ) );
		assert( isOk );
	}

	accessor = preference->findProperty( "layoutState" );
	if (accessor.isValid())
	{
		std::shared_ptr< BinaryBlock > state;
		bool isOk = preference->get( "layoutState", state );
		assert( isOk );
		isOk = mainWindow_->restoreState( QByteArray( state->cdata(), static_cast<int>(state->length()) ) );
		assert( isOk );
	}
	
	bool isMaximized = false;
	accessor = preference->findProperty( "maximized" );
	if (!accessor.isValid())
	{
		return false;
	}
	bool isOk = preference->get( "maximized", isMaximized );
	assert( isOk );
	if (isMaximized)
	{
		mainWindow_->showMaximized();
	}
	else
	{
		Vector2 pos;
		isOk = preference->get( "pos", pos );
		if (isOk)
		{
			mainWindow_->move( QPoint( static_cast<int>( pos.x ), static_cast<int>( pos.y ) ) );
		}

		Vector2 size;
		isOk = preference->get( "size", size );
		if (isOk)
		{
			mainWindow_->resize( QSize( static_cast<int>( size.x ), static_cast<int>( size.y ) ) );
		}
		mainWindow_->show();
	}
	return true;
}
