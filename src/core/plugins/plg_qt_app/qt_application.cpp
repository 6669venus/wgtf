#include "qt_application.hpp"

#include "core_automation/interfaces/automation_interface.hpp"
#include "core_common/ngt_windows.hpp"
#include "core_common/environment.hpp"

#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/qml_view.hpp"
#include "core_qt_common/qt_palette.hpp"
#include "core_qt_common/qt_window.hpp"

#include "core_ui_framework/i_action.hpp"

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

#ifdef _WIN32
	application_.reset( new QApplication( __argc, __argv ) );
#else
	application_.reset( new QApplication( argc, argv ) );
#endif

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
	if(qtFramework_ != nullptr)
	{
		auto palette = qtFramework_->palette();
		if (palette != nullptr)
		{
			application_->setPalette( palette->toQPalette() );
		}
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
	signalOnUpdate_.clear();
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

	signalOnUpdate_();

}

int QtApplication::startApplication()
{
	assert( application_ != nullptr );

	return application_->exec();
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

void QtApplication::connectOnUpdate(VoidCallback callback)
{
	signalOnUpdate_.connect(callback);
}

/*
void QtApplication::getCommandLine()
{
	char* winCommandLine = GetCommandLine();
	std::vector<char*> parameters;
	char* position = winCommandLine;
	char* start = position;
	bool quote = false;
	bool space = false;
	size_t index = 0;

	auto copyData = [&position, &start]()->char*
	{
		size_t len = position - start;
		char* destination = new char[len + 1];
		memcpy(destination, start, len);
		destination[len] = 0;
		return destination;
	};

	while (*position)
	{
		switch (*position)
		{
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			if (!quote)
			{
				parameters.push_back(copyData());
				++index;
				space = true;
			}

			break;

		case '"':
			if (!quote)
			{
				start = position;
			}

			space = false;
			quote = !quote;
			break;

		default:
			if (space)
			{
				start = position;
			}

			space = false;
		}

		++position;
	}

	if (position > winCommandLine && !whiteSpace(*(position - 1)))
	{
		parameters.push_back(copyData());
		++index;
	}

	argc = index;
	argv = new char*[argc + 1];
	argv[argc] = nullptr;

	for (unsigned int i = 0; i < index; ++i)
	{
		argv[i] = parameters[i];
	}
}

bool QtApplication::whiteSpace(char c)
{
	return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}
*/
