#include "qt_application.hpp"

#include "automation/interfaces/automation_interface.hpp"

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
	: application_( new QApplication( __argc, __argv ) )
	, qtFramework_( nullptr )
{
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