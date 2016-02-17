#include "qml_window.hpp"
#include "qt_dock_region.hpp"
#include "qt_menu_bar.hpp"
#include "qt_status_bar.hpp"
#include "qt_tab_region.hpp"
#include "qt_tool_bar.hpp"
#include "i_qt_framework.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_preferences.hpp"
#include "core_logging/logging.hpp"
#include <cassert>
#include <thread>
#include <chrono>
#include <QQmlComponent>
#include <QDockWidget>
#include <QQuickWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QToolBar>
#include <QQmlContext>
#include <QQmlEngine>
#include <QApplication>
#include <QWindow>
#include <QElapsedTimer>

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

QmlWindow::QmlWindow( IQtFramework & qtFramework, QQmlEngine & qmlEngine )
	: qtFramework_( qtFramework )
	, qmlContext_( new QQmlContext( qmlEngine.rootContext() ) )
	, mainWindow_( new QQuickWidget( &qmlEngine, nullptr ) )
	, released_( false )
	, application_( nullptr )
{
	mainWindow_->setMinimumSize( QSize( 100, 100 ) );
	QQmlEngine::setContextForObject( mainWindow_, qmlContext_.get() );
}

QmlWindow::~QmlWindow()
{
	if (!released_)
	{
		mainWindow_->removeEventFilter( this );
		delete mainWindow_;
	}
}

void QmlWindow::setContextObject( QObject * object )
{
	qmlContext_->setContextObject( object );
}

void QmlWindow::setContextProperty(
	const QString & name, const QVariant & property )
{
	qmlContext_->setContextProperty( name, property );
}

void QmlWindow::error( QQuickWindow::SceneGraphError error, const QString &message )
{
	NGT_ERROR_MSG( "QmlWindow::error, rendering error: %s\n",
		message.toLatin1().constData() );
}

const char * QmlWindow::id() const
{
	return id_.c_str();
}

const char * QmlWindow::title() const
{
	return title_.c_str();
}

void QmlWindow::update()
{
	for (auto & menu : menus_)
	{
		menu->update();
	}
}

void QmlWindow::close()
{
	mainWindow_->close();
}

void QmlWindow::setIcon(const char* path)
{
	if(!path || !mainWindow_)
		return;
	mainWindow_->setWindowIcon(QIcon(path));
}

void QmlWindow::show( bool wait /* = false */)
{
	mainWindow_->setWindowModality( modalityFlag_ );

	mainWindow_->show();
	if ( title() )
	{
		mainWindow_->setWindowTitle(title());
	}
	if (wait)
	{
		waitForWindowExposed();
	}
}

void QmlWindow::showMaximized( bool wait /* = false */)
{
	mainWindow_->setWindowModality( modalityFlag_ );
	
	mainWindow_->showMaximized();
	if (wait)
	{
		waitForWindowExposed();
	}
}

void QmlWindow::showModal()
{
	mainWindow_->setWindowModality( Qt::ApplicationModal );
	if ( title() )
	{
		mainWindow_->setWindowTitle(title());
	}
	mainWindow_->show();
}

void QmlWindow::hide()
{
	mainWindow_->hide();
}

const Menus & QmlWindow::menus() const
{
	return menus_;
}

const Regions & QmlWindow::regions() const
{
	return regions_;
}

void QmlWindow::setApplication( IUIApplication * application )
{
	application_ = application;
}

IUIApplication * QmlWindow::getApplication() const
{
	return application_;
}

IStatusBar* QmlWindow::statusBar() const
{
	return statusBar_.get();
}

QQuickWidget * QmlWindow::release()
{
	released_ = true;
	return window();
}

QQuickWidget * QmlWindow::window() const
{
	return mainWindow_;
}

void QmlWindow::waitForWindowExposed()
{
	if (mainWindow_ == nullptr)
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

bool QmlWindow::load( QUrl & qUrl )
{
	auto qmlEngine = qmlContext_->engine();
	auto qmlComponent = std::unique_ptr< QQmlComponent >(
		new QQmlComponent( qmlEngine, qUrl, mainWindow_ ) );
	assert( !qmlComponent->isLoading() );
	if (!qmlComponent->isReady())
	{
		NGT_WARNING_MSG( "Error loading control %s\n",
			qPrintable( qmlComponent->errorString() ) );
		return false;
	}

	auto content = std::unique_ptr< QObject >(
		qmlComponent->create( qmlContext_.get() ) );

	QVariant windowProperty = content->property( "id" );
	if (windowProperty.isValid())
	{
		id_ = windowProperty.toString().toUtf8().data();
	}

	QVariant titleProperty = content->property( "title" );
	if (titleProperty.isValid())
	{
		title_ = titleProperty.toString().toUtf8().data();
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
	if (!dockWidgets.empty())
	{
		NGT_WARNING_MSG( "Qml window doesn't support docking" );
	}
	auto tabWidgets = getChildren< QTabWidget >( *mainWindow_ );
	for (auto & tabWidget : tabWidgets)
	{
		if ( tabWidget->property( "layoutTags" ).isValid() )
		{
			regions_.emplace_back( new QtTabRegion( qtFramework_, *tabWidget ) );
		}
	}
	auto statusBar = getChildren<QStatusBar>(*mainWindow_);
	if ( statusBar.size() > 0 )
	{
		statusBar_.reset(new QtStatusBar(*statusBar.at(0)));
	}

	auto preferences = qtFramework_.getPreferences();
	auto preference = preferences->getPreference( id_.c_str() );
	auto value = qtFramework_.toQVariant( preference );
	this->setContextProperty( QString( "Preference" ), value );

	mainWindow_->setContent( qUrl, qmlComponent.release(), content.release() );
	mainWindow_->setResizeMode( QQuickWidget::SizeRootObjectToView );
	QObject::connect( mainWindow_, SIGNAL(sceneGraphError(QQuickWindow::SceneGraphError, const QString&)),
		this, SLOT(error(QQuickWindow::SceneGraphError, const QString&)) );
	mainWindow_->installEventFilter( this );
	modalityFlag_ = mainWindow_->windowModality();
	return true;
}

bool QmlWindow::eventFilter( QObject * object, QEvent * event )
{
	if (object == mainWindow_)
	{
		if (event->type() == QEvent::Close)
		{
			this->onClose();
			return true;
		}
	}
	return QObject::eventFilter( object, event );
}
