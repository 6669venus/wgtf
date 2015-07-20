#include "qt_application.hpp"

#include "automation/interfaces/automation_interface.hpp"
#include "ngt_core_common/environment.hpp"

#include "qt_common/i_qt_framework.hpp"
#include "qt_common/qml_view.hpp"
#include "qt_common/qt_palette.hpp"
#include "qt_common/qt_window.hpp"

#include "ui_framework/i_action.hpp"

#include <cassert>

#include <QAbstractEventDispatcher>
#include <QApplication>
#include <QFont>
#include <QPalette>
#include <QStyleFactory>
#include <QTimer>

namespace
{
	class IdleLoop : public QObject
	{
	public:
		IdleLoop( QtApplication & qtApplication, QObject * parent )
			: QObject( parent )
			, qtApplication_( qtApplication )
		{
			timer_ = new QTimer( this );
			timer_->setInterval( 0 );
			QObject::connect( timer_, &QTimer::timeout, [&]() { 
				qtApplication_.update(); 
			} );
		}

	public slots:
		void start()
		{
			qtApplication_.update();
			timer_->start();
		}

		void stop()
		{
			timer_->stop();
		}

	private:
		QtApplication & qtApplication_;
		QTimer * timer_;
	};
}

QtApplication::QtApplication()
	: application_( nullptr )
	, qtFramework_( nullptr )
{
	char ngtHome[MAX_PATH];

	if (Environment::getValue<MAX_PATH>( "NGT_HOME", ngtHome ))
	{
		application_->addLibraryPath( ngtHome );
		//application_->addLibraryPath( QString( ngtHome ) + "\\platforms" );
		Environment::setValue( "QT_QPA_PLATFORM_PLUGIN_PATH", (std::string( ngtHome ) + "/platforms").c_str() );
	}

	application_.reset( new QApplication( __argc, __argv ) );

	QCoreApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
	QApplication::setDesktopSettingsAware( false );
	QApplication::setStyle( QStyleFactory::create( "Fusion" ) );
	QApplication::setFont( QFont( "Noto Sans", 9 ) );

	auto dispatcher = QAbstractEventDispatcher::instance();
	auto idleLoop = new IdleLoop( *this, application_.get() );
	QObject::connect( dispatcher, &QAbstractEventDispatcher::aboutToBlock,
		idleLoop, &IdleLoop::start );
	QObject::connect( dispatcher, &QAbstractEventDispatcher::awake,
		idleLoop, &IdleLoop::stop );
	
}

QtApplication::~QtApplication()
{
}

void QtApplication::initialise( IQtFramework * qtFramework )
{
	qtFramework_ = qtFramework;
	assert( qtFramework_ != nullptr );

	auto palette = qtFramework_->palette();
	if (palette != nullptr)
	{
		application_->setPalette( palette->toQPalette() );
	}

	QObject::connect( application_.get(), &QGuiApplication::applicationStateChanged, [&]( Qt::ApplicationState state ) {
		if (state == Qt::ApplicationActive)
		{
			for(auto & listener : listeners_)
			{
				listener->applicationStarted();
			}
		}
	});

	QObject::connect( application_.get(), &QCoreApplication::aboutToQuit, [&]() {
		for(auto & listener : listeners_)
		{
			listener->applicationStopped();
		}
	});
}

void QtApplication::finalise()
{

}

void QtApplication::update()
{
	AutomationInterface* pAutomation =
		Context::queryInterface< AutomationInterface >();
	if (pAutomation)
	{
		if (pAutomation->timedOut())
		{
			QApplication::quit();
		}
	}

	layoutManager_.update();
}

int QtApplication::startApplication()
{
	assert( application_ != nullptr );

	return application_->exec();
}

void QtApplication::processEvents()
{
	application_->processEvents();
}

void QtApplication::registerListener( IApplicationListener * listener )
{
	listeners_.push_back( listener );
}


void QtApplication::deregisterListener( IApplicationListener * listener )
{
	auto && listenerIt = std::find( listeners_.begin(), listeners_.end(), listener );
	assert( listenerIt != listeners_.end() );
	listeners_.erase( listenerIt );
}


void QtApplication::addWindow( IWindow & window )
{
	layoutManager_.addWindow( window );
}

void QtApplication::addView( IView & view )
{
	layoutManager_.addView( view );
}

void QtApplication::addAction( IAction & action )
{
	layoutManager_.addAction( action );
}

const Windows & QtApplication::windows() const
{
	return layoutManager_.windows();
}
